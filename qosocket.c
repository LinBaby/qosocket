/* qosocket extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_qosocket.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ zend_class_entry
 */
zend_class_entry *qst_p;
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(qst_ctor_args, 0, 0, 1)
    ZEND_ARG_INFO(0, ipproto)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(qst_bind_args, 0, 0, 2)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
ZEND_END_ARN_INFO()

PHP_METHOD(qosocket, __construct);
PHP_METHOD(qosocket, bind);

/* {{{ qst_methods[]
 */
const zend_function_entry qst_methods[] = {
    PHP_ME(qosocket, __construct, qst_ctor_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR);
    PHP_ME(qosocket, bind, qst_bind_args, ZEND_ACC_PUBLIC);
}
/* }}} */

void init_class();

/* {{{ qosocket 类
 */
PHP_MINIT_FUNCTION(qosocket)
{
    init_class();
    return SUCCESS;
}
/* }}} */

/*  {{{ init_class()
 */
void init_class()
{
    zend_class_entry qst;
    INIT_CLASS_ENTRY(qst, "Qst\\Http\\Server", qst_methods);
    qst_p = zend_register_internal_class(&qst TSRMLS_CC);
    // 创建host属性
    zend_declare_property_string(
        qsocket_p,
        ZEND_STRL(QS_HOST),
        QS_DEFAULT_HOST,
        ZEND_ACC_PROTECTED
    );
    // 创建port属性
    zend_declare_property_long(
        qst_p,
        ZEND_STRL(QS_PORT),
        QS_DEFAULT_PORT,
        ZEND_ACC_PROTECTED
    );
    // 创建ipproto
    zend_declare_property_long(
        qst_p,
        ZEND_STRL(QS_IPPROTO),
        IPPROTO_TCP,
        ZEND_ACC_PROTECTED
    );
}
/* }}} */

/* {{{ qosocket::__construct()
 */
PHP_METHOD(qosocket, __construct)
{
    long ipproto;
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(ipproto)
    ZEMD_PARSE_PARAMETERS_END();
    zend_update_proper_long(
        qst_p, getThis(), ZEND_STR(QS_IPPROTO), ipproto
    );
}
/* }}} */

/* {{{ qosocket::bind(host, port)
 */
PHP_METHOD(qosocket, bind)
{
    char  * host;
    size_t  host_len;
    long    port;
    zval  * this = getThis();

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(host, host_len)
        Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property_string(
        qst_p, this, QS_HOST, QS_HOST_LEN, host
    );

    zend_update_property_long(
        qst_p, this, QS_PORT, QS_PORT_LEN, port
    );
}
/* }}} */

/* {{{ void qosocket_test1()
 */
PHP_FUNCTION(qosocket_test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "qosocket");
}
/* }}} */

/* {{{ string qosocket_test2( [ string $var ] )
 */
PHP_FUNCTION(qosocket_test2)
{
	char *var = "World";
	size_t var_len = sizeof("World") - 1;
	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(var, var_len)
	ZEND_PARSE_PARAMETERS_END();

	retval = strpprintf(0, "Hello %s", var);

	RETURN_STR(retval);
}
/* }}}*/

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(qosocket)
{
#if defined(ZTS) && defined(COMPILE_DL_QOSOCKET)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(qosocket)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "qosocket support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_qosocket_test1, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_qosocket_test2, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ qosocket_functions[]
 */
static const zend_function_entry qosocket_functions[] = {
	PHP_FE(qosocket_test1,		arginfo_qosocket_test1)
	PHP_FE(qosocket_test2,		arginfo_qosocket_test2)
	PHP_FE_END
};
/* }}} */

/* {{{ qosocket_module_entry
 */
zend_module_entry qosocket_module_entry = {
	STANDARD_MODULE_HEADER,
	"qosocket",                /* Extension name */
    qosocket_functions,        /* zend_function_entry */
	PHP_MINIT(qosocket),       /* PHP_MINIT - Module initialization */
	NULL,                      /* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(qosocket),       /* PHP_RINIT - Request initialization */
	NULL,                      /* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(qosocket),       /* PHP_MINFO - Module info */
	PHP_QOSOCKET_VERSION,      /* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_QOSOCKET
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(qosocket)
#endif
