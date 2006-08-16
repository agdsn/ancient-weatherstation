<?
include_once("php_inc/module.inc.php");
include_once("php_inc/parser.inc.php");


/* Klasse, die die ModuleSets Verwaltet */
class ModuleSet{
  var $connInstance   = NULL;		/* Instanz der Verbindungs-Klasse */
  var $parserInstance = NULL;		/* Instanz des Parsers */

  /* Konstruktor */
  function ModuleSet($setName){
    $parser = $this->_getParserInstance();					/* Parserinstanz holen */
    $parser->parseContent($this->_getSetFilename($setName), $this, NULL);	/* Set Parsen */
  }

  /* Dateinamen eines Setz aus dessen Namen zusammenbauen */
  function _getSetFilename($setName){
    return "content/module_sets/set_".$setName.".html";
  }
  
  /* Parser Instanzieren (wenn noch nicht ist) und zurückgeben */
  function _getParserInstance(){
    if($this->parserInstance==NULL)
      $parserInstance = new Parser();
    return $parserInstance;
  }

  /* Ein Modul hinzufügen */
  function _addModule($modName){
    $params =  explode("_",$modName);				/* Modulname und Sensorid trennen */
    include("content/modules/mod_".$params[0].".html");
  }
}
?>
