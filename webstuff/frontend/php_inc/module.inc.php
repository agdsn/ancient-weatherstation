<?
include_once("php_inc/parser.inc.php");		/* Parser */
include_once("php_inc/sensor.inc.php");		/* Sensor-Klasse */
include_once("php_inc/temp.inc.php");		/* Temp-Klasse */
include_once("php_inc/rain.inc.php");		/* Rain-Klasse */

/* Representiert ein Modul */
class Module{

  var $sensId;				/* Sensor-Id */
  var $connection;			/* Connection - Instanz */
  var $parserInstance     = NULL;	/* Parser - Instanz */
  var $connectionInstance = NULL;	/* Connection - Instanz */
  var $sensInstance	  = NULL;	/* Sensor-Instanz */
  var $tempInstance	  = NULL;	/* Temp-Instanz */
  var $rainInstance	  = NULL;	/* Rain-Instanz */

  /* Konstruktor */
  function Module($modName, $sensId, &$parser, &$connection){
    
    /* Klassenvariablen zuordnen */
    $this->sensId     = $sensId;
    $this->connection = &$connection;
    $this->parserInstance = &$parser;

    $parser->parseContent($this->_getModuleFilename("frame"), & $this, "top");		/* Oberen Modulrahmen parsen */
    $parser->parseContent($this->_getModuleFilename($modName), & $this, NULL); 		/* Modul Parsen */
    $parser->parseContent($this->_getModuleFilename("frame"), & $this, "bottom"); 	/* unteren Modulrahmen Parsen */
  }

  /* Dateinamen des Modul-Files zusammenbauen */
  function _getModuleFilename($modName){
    return "content/modules/mod_".$modName.".html";
  }

  /* Instanz der Sensorklasse holen */
  function &_get_sens(){
    if($this->sensInstance == NULL)
      $this->sensInstance = new Sensor($this->sensId, $this->connection);
    return $this->sensInstance;
  }

  /* Instanz der Temp-Klasse holen */
  function &_get_temp(){
    if($this->tempInstance == NULL)
      $this->tempInstance = new Temp($this->sensId, $this->connection);
    return $this->tempInstance;
  }

  /* Instanz der Rain-Klasse holen */
  function &_get_rain(){
    if($this->rainInstance == NULL)
      $this->rainInstance = new Rain($this->sensId, $this->connection);
    return $this->rainInstance;
  }

  /* Callback-Funktion, wird ausgeführt wenn {content:fill:xyz} gefunden wird */
  function fill($contentId){
    $content_split = explode("_", $contentId);										/* Modultyp bekommen */
    $callObject	   = & call_user_method("_get_".$content_split[0], $this);						/* Instanz der zum Modul gehörenden Klasse */
    $funcName      = "get".substr($contentId, strlen($content_split[0]), strlen($contentId)-strlen($content_split[0])); /* Namen der In der Instanz aufzurufenden Methode zusammenbauen */
    
    return $callObject->$funcName($content_split[1]);									/* Methode ausführen (Wert holen) und zurückgeben */
  }

}
?>
