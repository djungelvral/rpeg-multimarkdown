#include "ruby.h"
#include "markdown_peg.h"

static VALUE rb_cMultiMarkdown;

int get_exts(VALUE self)
{
    int extensions = 0;
    if ( rb_funcall(self, rb_intern("smart"), 0) == Qtrue )
        extensions = extensions | EXT_SMART ;
    if ( rb_funcall(self, rb_intern("notes"), 0) == Qtrue )
        extensions = extensions | EXT_NOTES ;
    if ( rb_funcall(self, rb_intern("filter_html"), 0) == Qtrue )
        extensions = extensions | EXT_FILTER_HTML ;
    if ( rb_funcall(self, rb_intern("filter_styles"), 0) == Qtrue )
        extensions = extensions | EXT_FILTER_STYLES ;
    if ( rb_funcall(self, rb_intern("process_html"), 0) == Qtrue )
        extensions = extensions | EXT_PROCESS_HTML;
    /* Compatibility overwrites all other extensions */
    if ( rb_funcall(self, rb_intern("compatibility"), 0) == Qtrue )
        extensions = EXT_COMPATIBILITY;
    return extensions;
}

static VALUE
rb_multimarkdown_to_html(int argc, VALUE *argv, VALUE self)
{
    /* grab char pointer to multimarkdown input text */
    VALUE text = rb_funcall(self, rb_intern("text"), 0);
    Check_Type(text, T_STRING);
    char * ptext = StringValuePtr(text);

    int extensions = get_exts(self);

    char *html = markdown_to_string(ptext, extensions, HTML_FORMAT);
    VALUE result = rb_str_new2(html);
    free(html);
    
    /* Take the encoding from the input, and apply to output */
    rb_enc_copy(result, text);
        
    return result;
}

static VALUE
rb_multimarkdown_to_latex(int argc, VALUE *argv, VALUE self)
{
    /* grab char pointer to multimarkdown input text */
    VALUE text = rb_funcall(self, rb_intern("text"), 0);
    Check_Type(text, T_STRING);
    char * ptext = StringValuePtr(text);
    
    int extensions = get_exts(self);

    char *latex = markdown_to_string(ptext, extensions, LATEX_FORMAT);
    VALUE result = rb_str_new2(latex);
    free(latex);

    /* Take the encoding from the input, and apply to output */
    rb_enc_copy(result, text);
        
    return result;
}

static VALUE
rb_multimarkdown_extract_metadata(VALUE self, VALUE key)
{
    /* grab char pointer to multimarkdown input text */
    VALUE text = rb_funcall(self, rb_intern("text"), 0);
    Check_Type(text, T_STRING);
    char * ptext = StringValuePtr(text);

    Check_Type(key, T_STRING);
    char * pkey = StringValuePtr(key);
    
    int extensions = get_exts(self);

    /* Display metadata on request */
    char *metadata = extract_metadata_value(ptext, extensions, pkey);
    int exists = metadata != NULL;
    /* metadata might be null, if not present */
    /* rb_str_new2 == rb_str_new_cstr */
    VALUE result = exists ? rb_str_new2(metadata) : Qnil;
    free(metadata);

    /* Take the encoding from the input, and apply to output */
    if(exists)
    {
        rb_enc_copy(result, text);
    }
    
    return result;
}

void Init_peg_multimarkdown()
{
    rb_cMultiMarkdown = rb_define_class("PEGMultiMarkdown", rb_cObject);
    /* 
     * Document-method: PEGMultiMarkdown#to_html
     * Return string containing HTML generated from `text` 
     */
    rb_define_method(rb_cMultiMarkdown, "to_html", rb_multimarkdown_to_html, -1);
    /* 
     * Document-method: PEGMultiMarkdown#to_latex
     * Return string containing latex generated from `text` 
     */
    rb_define_method(rb_cMultiMarkdown, "to_latex", rb_multimarkdown_to_latex, -1);
    /* 
     * Document-method: PEGMultiMarkdown#extract_metadata
     * call-seq:
     *     extract_metadata(key)
     * Extract metadata specified by `key` from `text`
     */
    rb_define_method(rb_cMultiMarkdown, "extract_metadata", rb_multimarkdown_extract_metadata, 1);
}

// vim: ts=4 sw=4
