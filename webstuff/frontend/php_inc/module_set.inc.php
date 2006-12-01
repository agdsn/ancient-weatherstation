<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */

include_once($path."php_inc/module.inc.php");
include_once($path."php_inc/parser.inc.php");
include_once($path."php_inc/connection.inc.php");
include_once($path."php_inc/config.inc.php");
include_once($path."php_inc/chart.inc.php");
include_once($path."php_inc/report.inc.php");


/* Klasse, die die ModuleSets Verwaltet */
class ModuleSet{
  var $connInstance   = NULL;		/* Instanz der Verbindungs-Klasse */
  var $parserInstance = NULL;		/* Instanz des Parsers */

  /* Konstruktor */
  function ModuleSet($setName){
    $parser = & $this->_getParserInstance();					/* Parserinstanz holen */
    $parser->parseContent($this->_getSetFilename($setName), &$this, NULL, true);	/* Set Parsen */

    if($_REQUEST['chartName'] != "" || $_REQUEST['rptName'] != ""){
      $parser->appendContent($this->getBackLink());
    }

    $parser->printContent();
    
    if ($this->connInstance != NULL){
      $this->connInstance->closeConn();
    }
  }

  /* Dateinamen eines Setz aus dessen Namen zusammenbauen */
  function _getSetFilename($setName){
    global $path;
    if($setName == "")
      $setName = Config::getDefaultSet();
    return $path."content/module_sets/set_".$setName.".html";
  }
  
  /* Parser Instanzieren (wenn noch nicht ist) und zurückgeben */
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

  /* Ein Modul hinzufügen */
  function addModule($modName){
    $params = explode("_",$modName);				/* Modulname und Sensorid trennen */
    new Module($params[0], $params[1], $this->_getParserInstance(), $this->_getConnInstance());
  }

  /* Ein Report hinzufügen */
  function addReport($rptName){
    new Report($rptName, $this->_getParserInstance(), $this->_getConnInstance());
  }

  function addChart($template){
    new Chart($template,  $this->_getParserInstance());
  }

  function getBackLink(){
    $buff  = '<center>';
    $buff .= '<div class="back_link_div">';
    $buff .= '<a class="back_link" href="';
    $buff .= $_SERVER['HTTP_REFERER'];
    $buff .= '">';
    $buff .= 'zur&uuml;ck';
    $buff .= "</a>";
    $buff .= "</div>";
    $buff .= '</center>';

    return $buff;
  }

  function addSetLink($setName){
    return ModuleSet::buildArgLink(array("setType"=>$setName));
  }

  function buildArgLink($optArray){
    $tempArg = $_GET;
    foreach ($optArray as $key => $value){
      $tempArg[$key] = $value;
    }

    $buff = $_SERVER['PHP_SELF'];
    $i = 1;
    foreach ($tempArg as $key => $value){
      if($i){
        $i--;
        $buff .= '?'.$key.'='.$value;
      } else {
        $buff .= '&'.$key.'='.$value;
      }
    }

    return $buff;
  }
}
?>
