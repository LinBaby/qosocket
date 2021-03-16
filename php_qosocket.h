/* qosocket extension for PHP */

#ifndef PHP_QOSOCKET_H
# define PHP_QOSOCKET_H

extern zend_module_entry qosocket_module_entry;
# define phpext_qosocket_ptr &qosocket_module_entry

# define PHP_QOSOCKET_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_QOSOCKET)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

# define QS_HOST         "host"
# define QS_HOST_LEN     4
# define QS_DEFAULT_HOST "0.0.0.0"
# define QS_PORT         "port"
# define QS_PORT_LEN     4
# define QS_DEFAULT_PORT 1215
# define QS_IPPROTO      "ipproto"

#endif	/* PHP_QOSOCKET_H */
