#include <ruby.h>
#include <bsdconv.h>

#define IBUFLEN 1024

void Init_bsdconv();
static VALUE m_new(VALUE, VALUE);
static VALUE m_conv(VALUE, VALUE);
static VALUE m_init(VALUE);
static VALUE m_conv_chunk(VALUE, VALUE);
static VALUE m_conv_chunk_last(VALUE, VALUE);
static VALUE m_conv_file(VALUE, VALUE, VALUE);
static VALUE m_info(VALUE);
static VALUE m_nil(VALUE);
static VALUE m_error(VALUE);

void Init_bsdconv(){
	VALUE Bsdconv = rb_define_class("Bsdconv", rb_cObject);
	rb_define_singleton_method(Bsdconv, "new", m_new, 1);
	rb_define_method(Bsdconv, "conv", m_conv, 1);
	rb_define_method(Bsdconv, "init", m_init, 0);
	rb_define_method(Bsdconv, "conv_chunk", m_conv_chunk, 1);
	rb_define_method(Bsdconv, "conv_chunk_last", m_conv_chunk_last, 1);
	rb_define_method(Bsdconv, "conv_file", m_conv_file, 2);
	rb_define_method(Bsdconv, "info", m_info, 0);
	rb_define_method(Bsdconv, "nil?", m_nil, 0);
	rb_define_method(Bsdconv, "error", m_error, 0);
	rb_define_const(Bsdconv, "FROM", INT2NUM(FROM));
	rb_define_const(Bsdconv, "INTER", INT2NUM(INTER));
	rb_define_const(Bsdconv, "TO", INT2NUM(TO));
}

static VALUE m_new(VALUE class, VALUE conversion){
	struct bsdconv_instance *ins;
	if(TYPE(conversion)!=T_STRING)
		ins=bsdconv_create("");
	else
		ins=bsdconv_create(RSTRING(conversion)->ptr);
	return Data_Wrap_Struct(class, 0, bsdconv_destroy, ins);
}

static VALUE m_conv(VALUE self, VALUE str){
	VALUE ret;
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	bsdconv_init(ins);
	ins->output_mode=BSDCONV_AUTOMALLOC;
	ins->input.data=RSTRING(str)->ptr;
	ins->input.len=RSTRING(str)->len;
	ins->input.flags=0;
	ins->flush=1;
	bsdconv(ins);
	ret=rb_str_new(ins->output.data, ins->output.len);
	free(ins->output.data);
	return ret;
}

static VALUE m_init(VALUE self){
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	bsdconv_init(ins);
	return Qtrue;
}

static VALUE m_conv_chunk(VALUE self, VALUE str){
	VALUE ret;
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	ins->output_mode=BSDCONV_AUTOMALLOC;
	ins->input.data=RSTRING(str)->ptr;
	ins->input.len=RSTRING(str)->len;
	ins->input.flags=0;
	bsdconv(ins);
	ret=rb_str_new(ins->output.data, ins->output.len);
	free(ins->output.data);
	return ret;
}

static VALUE m_conv_chunk_last(VALUE self, VALUE str){
	VALUE ret;
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	ins->output_mode=BSDCONV_AUTOMALLOC;
	ins->input.data=RSTRING(str)->ptr;
	ins->input.len=RSTRING(str)->len;
	ins->input.flags=1;
	bsdconv(ins);
	ret=rb_str_new(ins->output.data, ins->output.len);
	free(ins->output.data);
	return ret;
}

static VALUE m_conv_file(VALUE self, VALUE ifile, VALUE ofile){
	struct bsdconv_instance *ins;
	FILE *inf, *otf;
	char *s1=RSTRING(ifile)->ptr;
	char *s2=RSTRING(ofile)->ptr;
	char *in;
	char *tmp;
	int fd;

	Data_Get_Struct(self, struct bsdconv_instance, ins);
	inf=fopen(s1,"r");
	if(!inf){
		return Qfalse;
	}
	tmp=malloc(strlen(s2)+8);
	strcpy(tmp, s2);
	strcat(tmp, ".XXXXXX");
	if((fd=mkstemp(tmp))==-1){
		free(tmp);
		return Qfalse;
	}
	otf=fdopen(fd,"w");
	if(!otf){
		free(tmp);
		return Qfalse;
	}

	bsdconv_init(ins);
	do{
		in=malloc(IBUFLEN);
		ins->input.data=in;
		ins->input.len=fread(in, 1, IBUFLEN, inf);
		ins->input.flags|=F_FREE;
		if(ins->input.len==0){
			ins->flush=1;
		}
		ins->output_mode=BSDCONV_FILE;
		ins->output.data=otf;
		bsdconv(ins);
	}while(ins->flush==0);

	fclose(inf);
	fclose(otf);
	unlink(s2);
	rename(tmp,s2);
	free(tmp);

	return Qtrue;
}

static VALUE m_info(VALUE self){
	VALUE ret;
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	ret=rb_hash_new();
	rb_hash_aset(ret, rb_str_new2("ierr"), INT2FIX(ins->ierr));
	rb_hash_aset(ret, rb_str_new2("oerr"), INT2FIX(ins->oerr));
	rb_hash_aset(ret, rb_str_new2("score"), INT2FIX(ins->score));
	return ret;
}

static VALUE m_nil(VALUE self){
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	if(ins==NULL)
		return Qtrue;
	return Qfalse;
}

static VALUE m_error(VALUE self){
	VALUE ret;
	char *s=bsdconv_error();
	ret=rb_str_new2(s);
	free(s);
	return ret;
}
