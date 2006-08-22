<?
  include_once("php_inc/module_set.inc.php");
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
  <head>
    <title>Wetter-Testfile</title>
    <link rel="stylesheet" type="text/css" href="css/example.css">
  </head>
  <body>
    <h1>Wetter-Testfile</h1>
	<? new ModuleSet($_REQUEST['setType']);?>
  </body>
</html>
