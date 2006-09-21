<?
include_once("php_inc/config.inc.php");
include_once("php_inc/module_set.inc.php");

class Report{

  var $tmplName;			/* Template-Id */
  var $rptName;				/* Report-Id */
  var $rptArray;
  var $connection;			/* Connection - Instanz */
  var $parserInstance     = NULL;	/* Parser - Instanz */
  var $connectionInstance = NULL;	/* Connection - Instanz */
  var $sensInstance	  = NULL;	/* Sensor-Instanz */

  /* Konstruktor */
  function Report($tplName, &$parser, &$connection){

    /* Klassenvariablen zuordnen */
    $this->tmplName       = $tplName;
    $this->rptName        = $_REQUEST['rptName'];
    $this->rptArray       = Config::getRptArray($this->rptName);
    $this->connInstance   = &$connection;
    $this->parserInstance = &$parser;

    $parser->parseContent($this->_getRptFilename($tplName), & $this, "top");          /* Oberen Modulrahmen parsen */
    $this->buildTable();
    $parser->parseContent($this->_getRptFilename($tplName), & $this, "bottom");       /* unteren Modulrahmen Parsen */
  }

  /* Dateinamen des Modul-Files zusammenbauen */
  function _getRptFilename($modName){
    return "content/reports/rpt_".$modName.".html";
  }

  
  function buildTable(){

    /* Tabelle des Sensors bestimmen */
    $tableQuery  = "SELECT tabelle FROM sensoren, typen WHERE sensoren.id=".$this->rptArray[2]." AND typen.typ = sensoren.typ";
    $table       = $this->connInstance->fetchQueryResultLine($tableQuery);
    
    if($this->rptArray[10]){
      $div = "/count(".$this->rptArray[3].")";
    } else {
      $div = "";
    }

    $result = &$this->connInstance->getRawResult("SELECT to_char(date_trunc('".$this->rptArray[4]."', timestamp), '".$this->rptArray[11]."') as ts, sum(".$this->rptArray[3].")".$div." as val FROM ".$table['tabelle']." WHERE sens_id=".$this->rptArray[2]." AND timestamp > (current_timestamp - INTERVAL '".$this->rptArray[5]."') GROUP BY ts ORDER BY max(timestamp) DESC");
        
    $buff  = '<table class="report_table">';
    $buff .= '<tr class="report_head"><th class="report_time">Zeit</th><th class="report_val">'.$this->rptArray[6].'</th></tr>';
    $buff .= "\n"; 

    $this->parserInstance->appendContent($buff);
     
    while($array = pg_fetch_assoc($result)){

      $buff  = '<tr class="report_line">';
      
      $buff .= '<td class="report_time">';
      $buff .= $array['ts'];
      $buff .= '</td>';
      
      $buff .= '<td class="report_val">';
      $buff .= round(($array['val'] * $this->rptArray[7]), 1).$this->rptArray[8]; 
      $buff .= '</td>';
      
      $buff .= '</tr>';
      
      $buff .= "\n";
      $this->parserInstance->appendContent($buff);

    }

    $buff = "</table>";
    $buff .= "\n";

    $this->parserInstance->appendContent($buff);
    

  }

  /* Ein Modul hinzufügen */
  function addModule($modName){
    new Module($this->rptArray[9], $this->rptArray[2], $this->_getParserInstance(), $this->_getConnInstance());
  }

  /* Callback-Funktion, wird ausgeführt wenn {content:fill:xyz} gefunden wird */
  function fill($contentId){
    $content_split = explode("_", $contentId);										/* Modultyp bekommen */
    $callObject	   = & call_user_method("_get_".$content_split[0], $this);						/* Instanz der zum Modul gehörenden Klasse */
    $funcName      = "get".substr($contentId, strlen($content_split[0]), strlen($contentId)-strlen($content_split[0])); /* Namen der In der Instanz aufzurufenden Methode zusammenbauen */
    
    return $callObject->$funcName($content_split[1]);									/* Methode ausführen (Wert holen) und zurückgeben */
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

  /* Instanz der Sensorklasse holen */
  function &_get_sens(){
    if($this->sensInstance == NULL)
      $this->sensInstance = new Sensor($this->sensId, & $this->connInstance);
    return $this->sensInstance;
  }

  function getReportLink($rptName){
    $rptArray = Config::getRptArray($rptName);

    $buff  = '<div class="report_link_div">';
    
    $buff .=  '<a href="';
    $buff .=  ModuleSet::buildArgLink(array("setType"=>$rptArray[12], "rptName"=>$rptName));
    $buff .=  '" class="report_link">';

    $buff .=  $rptArray[0];
    $buff .=  '<p class="report_link_desc">';
    $buff .=  $rptArray[1];
    $buff .=  '</p>';

    $buff .=  '</a>';
    $buff .=  '</div>';

    return $buff;
  }

}
?>
