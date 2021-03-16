--TEST--
qosocket_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('qosocket')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = qosocket_test1();

var_dump($ret);
?>
--EXPECT--
The extension qosocket is loaded and working!
NULL
