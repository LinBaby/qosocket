/* qosocket extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#include "php.h"
#include "ext/standard/info.h"
#include "php_qosocket.h"

#include "wrap.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ zend_class_entry
 */
zend_class_entry *qs_p;
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(qs_ctor_args, 0, 0, 1)
    ZEND_ARG_INFO(0, ipproto)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(qs_bind_args, 0, 0, 2)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
ZEND_END_ARN_INFO()

PHP_METHOD(qosocket, __construct);
PHP_METHOD(qosocket, bind);
PHP_METHOD(qosocket, run);

/* {{{ qs_methods[]
 */
const zend_function_entry qs_methods[] = {
    PHP_ME(qosocket, __construct, qs_ctor_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR);
    PHP_ME(qosocket, bind, qs_bind_args, ZEND_ACC_PUBLIC);
    PHP_ME(qosocket, run, NULL, ZEND_ACC_PUBLIC);
}
/* }}} */

// 初始化类
void init_class();
// 监听子进程,防止出现僵尸进程
void wait_child(int signo);

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
    zend_class_entry qs;
    INIT_CLASS_ENTRY(qs, "Qst\\Http\\Server", qs_methods);
    qs_p = zend_register_internal_class(&qs TSRMLS_CC);
    // 创建host属性
    zend_declare_property_string(
        qsocket_p,
        ZEND_STRL(QS_HOST),
        QS_DEFAULT_HOST,
        ZEND_ACC_PROTECTED
    );
    // 创建port属性
    zend_declare_property_long(
        qs_p, ZEND_STRL(QS_PORT), QS_DEFAULT_PORT, ZEND_ACC_PROTECTED
    );
    // 创建ipproto
    zend_declare_property_long(
        qs_p, ZEND_STRL(QS_IPPROTO), IPPROTO_TCP, ZEND_ACC_PROTECTED
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
        qs_p, getThis(), ZEND_STR(QS_IPPROTO), ipproto
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
        qs_p, this, QS_HOST, QS_HOST_LEN, host
    );

    zend_update_property_long(
        qs_p, this, QS_PORT, QS_PORT_LEN, port
    );
}
/* }}} */

/* {{{ qosocket::run()
 */
PHP_METHOD(qosocket, run)
{
	pid_t pid;
    int lfd, cfd;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_len;
    char buf[128] = {0}, clnt_ip[128] = {0};
    int n, i;

    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(QS_PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    Listen(lfd, QS_LISTEN);

    while (1) {
        clnt_addr_len = sizeof(serv_addr);
        cfd = Accept(lfd, (struct sockaddr *)&clnt_addr, &clnt_addr_len);

        printf("Clint IP:%s, port: %d \n", 
            inet_ntop(
                AF_INET, 
                &clnt_addr.sin_addr.s_addr,
                clnt_ip,
                sizeof(clnt_ip)
            ), 
            ntohs(clnt_addr.sin_port)
        );

        pid = fork();
        if (pid < 0) {
            perr_exit("Fork error");
        } else if (pid == 0) {
            close(lfd);
            break;
        } else {
            close(cfd);
            signal(SIGCHLD, wait_child);
        } // end if
    } // end while

    while (1) {
       n = Read(cfd, buf, sizeof(buf));
       if (n == 0) {
           close(cfd);
        return n;
       } else if (n == -1) {
           perr_exit("Read error");
       } else {
           for (i = 0; i < n; i++) {
               buf[i] = toupper(buf[i]);
           }
           Write(cfd, buf, n);
           Write(STDOUT_FILENO, buf, n);
       } // end if
    } // end while
}
/* }}} */

void wait_child(int signo)
{
    while (waitpid(0, NULL, WNOHANG) > 0);
    return ;
}

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
