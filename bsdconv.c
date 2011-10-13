#include <ruby.h>
#include <bsdconv.h>

void Init_bsdconv();
static VALUE m_new(VALUE, VALUE);
static VALUE m_conv(VALUE, VALUE);

void Init_bsdconv(){
	VALUE Bsdconv = rb_define_class("Bsdconv", rb_cObject);
	rb_define_singleton_method(Bsdconv, "new", m_new, 1);
	rb_define_method(Bsdconv, "conv", m_conv, 1);
}

static VALUE m_new(VALUE class, VALUE conversion){
	struct bsdconv_instance *ins;
	ins=bsdconv_create(RSTRING(conversion)->ptr);
	if(ins==NULL)
		return Qnil;
	return Data_Wrap_Struct(class, 0, bsdconv_destroy, ins);
	
}

static VALUE m_conv(VALUE self, VALUE str){
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	bsdconv_init(ins);
	ins->output_mode=BSDCONV_AUTOMALLOC;
	ins->input.data=RSTRING(str)->ptr;
	ins->input.len=RSTRING(str)->len;
	ins->input.flags=0;
	ins->flush=1;
	bsdconv(ins);
	return rb_str_new(ins->output.data, ins->output.len);
}
