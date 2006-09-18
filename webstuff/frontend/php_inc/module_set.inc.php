<?
include_once("php_inc/module.inc.php");
include_once("php_inc/parser.inc.php");
include_once("php_inc/connection.inc.php");
include_once("php_inc/config.inc.php");
include_once("php_inc/chart.inc.php");


/* Klasse, die die ModuleSets Verwaltet */
class ModuleSet{
  var $connInstance   = NULL;		/* Instanz der Verbindungs-Klasse */
  var $parserInstance = NULL;		/* Instanz des Parsers */

  /* Konstruktor */
  function ModuleSet($setName){
    $parser = & $this->_getParserInstance();					/* Parserinstanz holen */
    $parser->parseContent($this->_getSetFilename($setName), &$this, NULL);	/* Set Parsen */

    if($_REQUEST['chartName'] != ""){
      $parser->appendContent($this->getBackLink());
    }

    $parser->printContent();
    
    if ($this->connInstance != NULL){
      $this->connInstance->closeConn();
    }
  }

  /* Dateinamen eines Setz aus dessen Namen zusammenbauen */
  function _getSetFilename($setName){
    if($setName == "")
      $setName = Config::getDefaultSet();
    return "content/module_sets/set_".$setName.".html";
  }
  
  /* Parser Instanzieren (wenn noch nicht ist) und zur�ckgeben */
  function &_getParserInstance(){
    if($this->parserInstance==NULL)
      $this->parserInstance = new Parser();
    return $this->parserInstance;
  }

  function &_getConnInstance(){
    if($connInstance == NULL){
      $this->connInstance = new Connection();
    }
    return $this->connInstance;
  }

  /* Ein Modul hinzuf�gen */
  function addModule($modName){
    $params = explode("_",$modName);				/* Modulname und Sensorid trennen */
    new Module($params[0], $params[1], $this->_getParserInstance(), $this->_getConnInstance());
  }

  function addChart($template){
    new Chart($template,  $this->_getParserInstance());
  }

  function getBackLink(){
    $buff  = '<div class="back_link_div">';
    $buff .= '<a class="back_link" href="';
    $buff .= $_SERVER['HTTP_REFERER'];
    $buff .= '">';
    $buff .= 'zur&uuml;ck';
    $buff .= "</a>";
    $buff .= "</div>";

    return $buff;
  }
}
?>
