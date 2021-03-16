/* qosocket extension for PHP */

#ifndef PHP_QOSOCKET_H
# define PHP_QOSOCKET_H

extern zend_module_entry qosocket_module_entry;
# define phpext_qosocket_ptr &qosocket_module_entry

# define PHP_QOSOCKET_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_QOSOCKET)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_QOSOCKET_H */
