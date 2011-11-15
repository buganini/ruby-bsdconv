#include <ruby.h>
#include <bsdconv.h>

#define IBUFLEN 1024

void Init_bsdconv();
static VALUE m_new(VALUE, VALUE);
static VALUE m_insert_phase(VALUE, VALUE, VALUE, VALUE);
static VALUE m_insert_codec(VALUE, VALUE, VALUE, VALUE);
static VALUE m_replace_phase(VALUE, VALUE, VALUE, VALUE);
static VALUE m_replace_codec(VALUE, VALUE, VALUE, VALUE);
static VALUE m_conv(VALUE, VALUE);
static VALUE m_init(VALUE);
static VALUE m_conv_chunk(VALUE, VALUE);
static VALUE m_conv_chunk_last(VALUE, VALUE);
static VALUE m_conv_file(VALUE, VALUE, VALUE);
static VALUE m_info(VALUE);
static VALUE m_nil(VALUE);
static VALUE m_inspect(VALUE);
static VALUE m_error(VALUE);

static VALUE f_codecs_list(VALUE);
static VALUE f_codec_check(VALUE, VALUE, VALUE);

void Init_bsdconv(){
	VALUE Bsdconv = rb_define_class("Bsdconv", rb_cObject);
	rb_define_singleton_method(Bsdconv, "new", m_new, 1);
	rb_define_method(Bsdconv, "insert_phase", m_insert_phase, 3);
	rb_define_method(Bsdconv, "insert_codec", m_insert_codec, 3);
	rb_define_method(Bsdconv, "replace_phase", m_replace_phase, 3);
	rb_define_method(Bsdconv, "replace_codec", m_replace_codec, 3);
	rb_define_method(Bsdconv, "conv", m_conv, 1);
	rb_define_method(Bsdconv, "init", m_init, 0);
	rb_define_method(Bsdconv, "conv_chunk", m_conv_chunk, 1);
	rb_define_method(Bsdconv, "conv_chunk_last", m_conv_chunk_last, 1);
	rb_define_method(Bsdconv, "conv_file", m_conv_file, 2);
	rb_define_method(Bsdconv, "info", m_info, 0);
	rb_define_method(Bsdconv, "nil?", m_nil, 0);
	rb_define_method(Bsdconv, "inspect", m_inspect, 0);
	rb_define_method(Bsdconv, "error", m_error, 0);
	rb_define_const(Bsdconv, "FROM", INT2NUM(FROM));
	rb_define_const(Bsdconv, "INTER", INT2NUM(INTER));
	rb_define_const(Bsdconv, "TO", INT2NUM(TO));

	rb_define_global_function("bsdconv_codecs_list", f_codecs_list, 0);
	rb_define_global_function("bsdconv_codec_check", f_codec_check, 2);
}

static VALUE m_new(VALUE class, VALUE conversion){
	struct bsdconv_instance *ins;
	if(TYPE(conversion)!=T_STRING)
		ins=bsdconv_create("");
	else
		ins=bsdconv_create(RSTRING_PTR(conversion));
	return Data_Wrap_Struct(class, 0, bsdconv_destroy, ins);
}

static VALUE m_insert_phase(VALUE self, VALUE conversion, VALUE phase_type, VALUE phasen){
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	return INT2NUM(bsdconv_insert_phase(ins, RSTRING_PTR(conversion), NUM2INT(phase_type), NUM2INT(phasen)));
}

static VALUE m_insert_codec(VALUE self, VALUE conversion, VALUE phasen, VALUE codecn){
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	return INT2NUM(bsdconv_insert_phase(ins, RSTRING_PTR(conversion), NUM2INT(phasen), NUM2INT(codecn)));
}

static VALUE m_replace_phase(VALUE self, VALUE conversion, VALUE phase_type, VALUE phasen){
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	return INT2NUM(bsdconv_insert_phase(ins, RSTRING_PTR(conversion), NUM2INT(phase_type), NUM2INT(phasen)));
}

static VALUE m_replace_codec(VALUE self, VALUE conversion, VALUE phasen, VALUE codecn){
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	return INT2NUM(bsdconv_insert_phase(ins, RSTRING_PTR(conversion), NUM2INT(phasen), NUM2INT(codecn)));
}

static VALUE m_conv(VALUE self, VALUE str){
	VALUE ret;
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	bsdconv_init(ins);
	ins->output_mode=BSDCONV_AUTOMALLOC;
	ins->input.data=RSTRING_PTR(str);
	ins->input.len=RSTRING_LEN(str);
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
	ins->input.data=RSTRING_PTR(str);
	ins->input.len=RSTRING_LEN(str);
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
	ins->input.data=RSTRING_PTR(str);
	ins->input.len=RSTRING_LEN(str);
	ins->input.flags=1;
	bsdconv(ins);
	ret=rb_str_new(ins->output.data, ins->output.len);
	free(ins->output.data);
	return ret;
}

static VALUE m_conv_file(VALUE self, VALUE ifile, VALUE ofile){
	struct bsdconv_instance *ins;
	FILE *inf, *otf;
	char *s1=RSTRING_PTR(ifile);
	char *s2=RSTRING_PTR(ofile);
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

static VALUE m_inspect(VALUE self){
#define TEMPLATE "Bsdconv.new(\"%s\")"
	VALUE ret;
	struct bsdconv_instance *ins;
	char *s;
	char *s2;
	int len=sizeof(TEMPLATE);
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	s=bsdconv_pack(ins);
	len+=strlen(s);
	s2=malloc(len);
	sprintf(s2, TEMPLATE, s);
	free(s);
	ret=rb_str_new2(s2);
	free(s2);
	return ret;
}

static VALUE m_error(VALUE self){
	VALUE ret;
	char *s=bsdconv_error();
	ret=rb_str_new2(s);
	free(s);
	return ret;
}

static VALUE f_codecs_list(VALUE self){
	int i;
	char **list, **p;
	VALUE ret;
	VALUE tmp;
	char *type[]={"from","inter","to"};
	ret=rb_hash_new();
	list=bsdconv_codecs_list();
	p=list;
	for(i=0;i<3;++i){
		tmp=rb_ary_new();
		while(*p!=NULL){
			rb_ary_push(tmp, rb_str_new2(*p));
			free(*p);
			p+=1;
		}
		rb_hash_aset(ret, rb_str_new2(type[i]), tmp);
		p+=1;
	}
	return ret;
}

static VALUE f_codec_check(VALUE self, VALUE phase_type, VALUE codec){
	if(bsdconv_codec_check(NUM2INT(phase_type), RSTRING_PTR(codec))){
		return Qtrue;
	}
	return Qfalse;
}
