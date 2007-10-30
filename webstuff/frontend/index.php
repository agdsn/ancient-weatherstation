<?
  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */

  $path = "./";
  include_once($path."php_inc/module_set.inc.php");
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8" >
    <title><? echo Config::getAllTitle(); ?></title>
    <link rel="stylesheet" type="text/css" href="css/example.css">
  </head>
  <body>
    <h1><? echo Config::getAllTitle(); ?></h1>
	<? new ModuleSet($_REQUEST['setType']);?>

	<p id="copyright">(c) 2006 by Jan Losinski</p>
  </body>
</html>
