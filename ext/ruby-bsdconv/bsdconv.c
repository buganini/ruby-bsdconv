/*
 * Copyright (c) 2011-2014 Kuan-Chung Chiu <buganini@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <ruby.h>
#ifdef HAVE_RUBY_IO_H		/* Ruby 1.9 and later */
#include "ruby/io.h"
#else				/* Ruby 1.8.x */
#include "rubyio.h"
#define rb_io_stdio_file(iot)	((iot)->f)
#endif
#include <bsdconv.h>

#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#define IBUFLEN 1024

void Init_bsdconv();
static VALUE m_new(VALUE, VALUE);
static VALUE m_conv(VALUE, VALUE);
static VALUE m_init(VALUE);
static VALUE m_ctl(VALUE, VALUE, VALUE, VALUE);
static VALUE m_conv_chunk(VALUE, VALUE);
static VALUE m_conv_chunk_last(VALUE, VALUE);
static VALUE m_conv_file(VALUE, VALUE, VALUE);
static VALUE m_counter(int, VALUE *, VALUE);
static VALUE m_counter_reset(int, VALUE *, VALUE);
static VALUE m_inspect(VALUE);

static VALUE f_insert_phase(VALUE, VALUE, VALUE, VALUE, VALUE);
static VALUE f_insert_codec(VALUE, VALUE, VALUE, VALUE, VALUE);
static VALUE f_replace_phase(VALUE, VALUE, VALUE, VALUE, VALUE);
static VALUE f_replace_codec(VALUE, VALUE, VALUE, VALUE, VALUE);
static VALUE f_error(VALUE);
static VALUE f_modules_list(VALUE, VALUE);
static VALUE f_module_check(VALUE, VALUE, VALUE);
static VALUE f_mktemp(VALUE, VALUE);
static VALUE f_fopen(VALUE, VALUE, VALUE);

VALUE Bsdconv_file;

void Init_bsdconv(){
	VALUE Bsdconv = rb_define_class("Bsdconv", rb_cObject);
	rb_define_singleton_method(Bsdconv, "new", m_new, 1);
	rb_define_method(Bsdconv, "conv", m_conv, 1);
	rb_define_method(Bsdconv, "init", m_init, 0);
	rb_define_method(Bsdconv, "ctl", m_ctl, 3);
	rb_define_method(Bsdconv, "conv_chunk", m_conv_chunk, 1);
	rb_define_method(Bsdconv, "conv_chunk_last", m_conv_chunk_last, 1);
	rb_define_method(Bsdconv, "conv_file", m_conv_file, 2);
	rb_define_method(Bsdconv, "counter", m_counter, -1);
	rb_define_method(Bsdconv, "counter_reset", m_counter_reset, -1);
	rb_define_method(Bsdconv, "inspect", m_inspect, 0);

	rb_define_const(Bsdconv, "FILTER", INT2NUM(FILTER));
	rb_define_const(Bsdconv, "FROM", INT2NUM(FROM));
	rb_define_const(Bsdconv, "INTER", INT2NUM(INTER));
	rb_define_const(Bsdconv, "TO", INT2NUM(TO));

	rb_define_const(Bsdconv, "CTL_ATTACH_SCORE", INT2NUM(BSDCONV_CTL_ATTACH_SCORE));
	rb_define_const(Bsdconv, "CTL_ATTACH_OUTPUT_FILE", INT2NUM(BSDCONV_CTL_ATTACH_OUTPUT_FILE));
	rb_define_const(Bsdconv, "CTL_AMBIGUOUS_PAD", INT2NUM(BSDCONV_CTL_AMBIGUOUS_PAD));

	rb_define_singleton_method(Bsdconv, "insert_phase", f_insert_phase, 4);
	rb_define_singleton_method(Bsdconv, "insert_codec", f_insert_codec, 4);
	rb_define_singleton_method(Bsdconv, "replace_phase", f_replace_phase, 4);
	rb_define_singleton_method(Bsdconv, "replace_codec", f_replace_codec, 4);
	rb_define_singleton_method(Bsdconv, "error", f_error, 0);
	rb_define_singleton_method(Bsdconv, "modules_list", f_modules_list, 1);
	rb_define_singleton_method(Bsdconv, "module_check", f_module_check, 2);
	rb_define_singleton_method(Bsdconv, "codecs_list", f_modules_list, 1);
	rb_define_singleton_method(Bsdconv, "codec_check", f_module_check, 2);
	rb_define_singleton_method(Bsdconv, "mktemp", f_mktemp, 1);
	rb_define_singleton_method(Bsdconv, "fopen", f_fopen, 2);

	Bsdconv_file = rb_define_class("Bsdconv_file", rb_cObject);
}

static VALUE m_new(VALUE class, VALUE conversion){
	struct bsdconv_instance *ins;
	if(TYPE(conversion)!=T_STRING)
		ins=bsdconv_create("");
	else
		ins=bsdconv_create(RSTRING_PTR(conversion));
	if(ins==NULL)
		return Qnil;
	return Data_Wrap_Struct(class, 0, bsdconv_destroy, ins);
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
	ins->input.next=NULL;
	ins->flush=1;
	bsdconv(ins);
	ret=rb_str_new(ins->output.data, ins->output.len);
	bsdconv_free(ins->output.data);
	return ret;
}

static VALUE m_init(VALUE self){
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	bsdconv_init(ins);
	return Qtrue;
}

static VALUE m_ctl(VALUE self, VALUE action, VALUE res, VALUE num){
	struct bsdconv_instance *ins;
	rb_io_t *fptr = NULL;
	void *ptr=NULL;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	if(TYPE(res)==T_FILE){
		GetOpenFile(res, fptr);
		ptr=rb_io_stdio_file(fptr);
	}else{
		Data_Get_Struct(res, FILE, ptr);
	}
	bsdconv_ctl(ins, NUM2INT(action), ptr, NUM2INT(num));
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
	ins->input.next=NULL;
	bsdconv(ins);
	ret=rb_str_new(ins->output.data, ins->output.len);
	bsdconv_free(ins->output.data);
	return ret;
}

static VALUE m_conv_chunk_last(VALUE self, VALUE str){
	VALUE ret;
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	ins->output_mode=BSDCONV_AUTOMALLOC;
	ins->input.data=RSTRING_PTR(str);
	ins->input.len=RSTRING_LEN(str);
	ins->input.flags=0;
	ins->input.next=NULL;
	ins->flush=1;
	bsdconv(ins);
	ret=rb_str_new(ins->output.data, ins->output.len);
	bsdconv_free(ins->output.data);
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

#ifndef WIN32
	struct stat stat;
	fstat(fileno(inf), &stat);
	fchown(fileno(otf), stat.st_uid, stat.st_gid);
	fchmod(fileno(otf), stat.st_mode);
#endif

	bsdconv_init(ins);
	do{
		in=bsdconv_malloc(IBUFLEN);
		ins->input.data=in;
		ins->input.len=fread(in, 1, IBUFLEN, inf);
		ins->input.flags|=F_FREE;
		ins->input.next=NULL;
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

static VALUE m_counter(int argc, VALUE *argv, VALUE self){
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	if(argc!=0){
		bsdconv_counter_t *v=bsdconv_counter(ins, RSTRING_PTR(argv[0]));
		return INT2NUM(*v);
	}else{
		VALUE ret=rb_hash_new();
		struct bsdconv_counter_entry *p=ins->counter;
		while(p){
			rb_hash_aset(ret, rb_str_new2(p->key), INT2FIX(p->val));
			p=p->next;
		}
		return ret;
	}
}

static VALUE m_counter_reset(int argc, VALUE *argv, VALUE self){
	struct bsdconv_instance *ins;
	Data_Get_Struct(self, struct bsdconv_instance, ins);
	if(argc!=0){
		bsdconv_counter_reset(ins, RSTRING_PTR(argv[0]));
	}else{
		bsdconv_counter_reset(ins, NULL);
	}
	return Qtrue;
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
	bsdconv_free(s);
	ret=rb_str_new2(s2);
	free(s2);
	return ret;
}

static VALUE f_insert_phase(VALUE self, VALUE conversion, VALUE codec, VALUE phase_type, VALUE phasen){
	char *s=bsdconv_insert_phase(RSTRING_PTR(conversion), RSTRING_PTR(codec), NUM2INT(phase_type), NUM2INT(phasen));
	VALUE ret=rb_str_new2(s);
	bsdconv_free(s);
	return ret;
}

static VALUE f_insert_codec(VALUE self, VALUE conversion, VALUE codec, VALUE phasen, VALUE codecn){
	char *s=bsdconv_insert_codec(RSTRING_PTR(conversion), RSTRING_PTR(codec), NUM2INT(phasen), NUM2INT(codecn));
	VALUE ret=rb_str_new2(s);
	bsdconv_free(s);
	return ret;
}

static VALUE f_replace_phase(VALUE self, VALUE conversion, VALUE codec, VALUE phase_type, VALUE phasen){
	char *s=bsdconv_replace_phase(RSTRING_PTR(conversion), RSTRING_PTR(codec), NUM2INT(phase_type), NUM2INT(phasen));
	VALUE ret=rb_str_new2(s);
	bsdconv_free(s);
	return ret;
}

static VALUE f_replace_codec(VALUE self, VALUE conversion, VALUE codec, VALUE phasen, VALUE codecn){
	char *s=bsdconv_replace_codec(RSTRING_PTR(conversion), RSTRING_PTR(codec), NUM2INT(phasen), NUM2INT(codecn));
	VALUE ret=rb_str_new2(s);
	bsdconv_free(s);
	return ret;
}

static VALUE f_error(VALUE self){
	VALUE ret;
	char *s=bsdconv_error();
	ret=rb_str_new2(s);
	bsdconv_free(s);
	return ret;
}

static VALUE f_modules_list(VALUE self, VALUE phase_type){
	char **list, **p;
	VALUE ret;
	ret=rb_ary_new();
	list=bsdconv_modules_list(NUM2INT(phase_type));
	p=list;
	while(*p!=NULL){
		rb_ary_push(ret, rb_str_new2(*p));
		bsdconv_free(*p);
		p+=1;
	}
	bsdconv_free(list);
	return ret;
}

static VALUE f_module_check(VALUE self, VALUE phase_type, VALUE codec){
	if(bsdconv_module_check(NUM2INT(phase_type), RSTRING_PTR(codec))){
		return Qtrue;
	}
	return Qfalse;
}

static VALUE f_mktemp(VALUE self, VALUE template){
	char *fn=strdup(RSTRING_PTR(template));
	int fd=bsdconv_mkstemp(fn);
	if(fd==-1)
		return Qnil;
	FILE *fp=fdopen(fd, "wb+");
	VALUE rfp=Data_Wrap_Struct(Bsdconv_file, 0, fclose, fp);
	VALUE ret;
	ret=rb_ary_new();
	rb_ary_push(ret, rfp);
	rb_ary_push(ret, rb_str_new2(fn));
	free(fn);
	return ret;
}

static VALUE f_fopen(VALUE self, VALUE filename, VALUE mode){
	FILE *fp=fopen(RSTRING_PTR(filename), RSTRING_PTR(mode));
	if(!fp)
		return Qnil;
	VALUE ret=Data_Wrap_Struct(Bsdconv_file, 0, fclose, fp);
	return ret;
}
