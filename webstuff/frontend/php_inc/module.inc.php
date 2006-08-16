<?
include_once("php_inc/parser.inc.php");

class Module{

  var $parserInstance     = NULL;	/* Parser - Instanz */
  var $connectionInstance = NULL;	/* Connection - Instanz */

  function Module($modName, $sensId, $parser, $connection){
    $parser->parseContent($this->_getModuleFilename($modName)); 
  }

  function _getModuleFilename($modName){
    return "/content/modules/mod_".$modName.".html";
  }

  function fill($contentId){
    $content_split = explode("_", $contentId);
    return $this->"get".$content_split[0]($content_split[1])
  }

}
?>
