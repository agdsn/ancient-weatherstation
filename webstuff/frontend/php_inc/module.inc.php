<?
include_once("php_inc/parser.inc.php");
include_once("php_inc/sensor.inc.php");
include_once("php_inc/temp.inc.php");

class Module{

  var $sensId;				/* Sensor-Id */
  var $connection;			/* Connection - Instanz */
  var $parserInstance     = NULL;	/* Parser - Instanz */
  var $connectionInstance = NULL;	/* Connection - Instanz */
  var $sensInstance	  = NULL;	/* Sensor-Instanz */
  var $tempInstance	  = NULL;	/* Temp-Instanz */


  function Module($modName, $sensId, &$parser, &$connection){
    $this->sensId     = $sensId;
    $this->connection = &$connection;
    $this->parserInstance = &$parser;


    $parser->parseContent($this->_getModuleFilename("frame"), & $this, "top"); 
    $parser->parseContent($this->_getModuleFilename($modName), & $this, NULL); 
    $parser->parseContent($this->_getModuleFilename("frame"), & $this, "bottom"); 
  }

  function _getModuleFilename($modName){
    return "content/modules/mod_".$modName.".html";
  }

  function &_get_sens(){
    if($this->sensInstance == NULL)
      $this->sensInstance = new Sensor($this->sensId, $this->connection);
    return $this->sensInstance;
  }

  function &_get_temp(){
    if($this->tempInstance == NULL)
      $this->tempInstance = new Temp($this->sensId, $this->connection);
    return $this->tempInstance;
  }

  function fill($contentId){
    $content_split = explode("_", $contentId);
    $callObject	   = & call_user_method("_get_".$content_split[0], $this);
    $funcName      = "get".substr($contentId, strlen($content_split[0]), strlen($contentId)-strlen($content_split[0]));
    
    

    return $callObject->$funcName($content_split[1]);
  }

}
?>
