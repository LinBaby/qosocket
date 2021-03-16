--TEST--
Check if qosocket is loaded
--SKIPIF--
<?php
if (!extension_loaded('qosocket')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "qosocket" is available';
?>
--EXPECT--
The extension "qosocket" is available
