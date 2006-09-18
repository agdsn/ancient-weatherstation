<?
include_once("php_inc/config.inc.php");

class Chart{

  function Chart($template, &$parser){
    $parser->parseContent($this->_getTemplateFileName("frame"), & $this, "top");          /* Oberen Modulrahmen parsen */
    $parser->parseContent($this->_getTemplateFileName($template), & $this, NULL);         /* Modul Parsen */
    $parser->parseContent($this->_getTemplateFileName("frame"), & $this, "bottom");       /* unteren Modulrahmen Parsen */
  }

  function _getTemplateFileName($template_name){
    return "content/charts/chart_".$template_name.".html";
  }

  function generateChartLink($chartName){
    $chartArray = Config::getChartArray($chartName);
    
    $buff  =  '<div class="chart_link_div">';
    
    $buff .=  '<a href="';
    $buff .=  $_SERVER['PHP_SELF'].'?setType='.$chartArray[4]."&chartName=".$chartName;
    $buff .=  '" class="chart_link">';
    
    $buff .=  $chartArray[1];
    $buff .=  '<p class="chart_link_desc">';
    $buff .=  $chartArray[2];

    if ($chartArray[3] == null){
      $buff .=  '<img class="chart_link_prev" width="250" height="100" src="';
      $buff .=  Chart::_getChartImgLink($chartArray[0]);
      $buff .=  '">';
    } else {
      $buff .=  '<img class="chart_link_prev" width="250" height="100" src="';
      $buff .=  Chart::_getChartImgLink($chartArray[3]);
      $buff .=  '">';
    }
    $buff .=  '</p>';

    $buff .=  '</a>';
    $buff .=  '</div>';
    return $buff;
  }

  function _getChartImgLink($img_filename){
    return Config::getDefaultChartDir().$img_filename;
  }

  function _getChartImgTag($chartName){
    $chartArray = Config::getChartArray($chartName);

    $buff  = '<img class="chart_img" src="';
    $buff .= Chart::_getChartImgLink($chartArray[0]);
    $buff .= '">';
   
    return $buff;
  }

  function insertChart($chartName){
    if($chartName == "auto"){
      $chartName = $_REQUEST['chartName'];
    }
    return Chart::_getChartImgTag($chartName);
  }

}

?>
