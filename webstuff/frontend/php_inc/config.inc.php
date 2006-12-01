<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */

if(file_exists("/etc/weatherstation/frontend.conf")){
  include_once("/etc/weatherstation/frontend.conf");
} else {
  include_once($path."frontend.conf"); 
}





/* Config-Klasse, Bitte nicht ändern! */
class Config{

  function getPgConnString(){
    global $pg_host,$pg_database,$pg_user,$pg_pass;
    return "host=".$pg_host." dbname=".$pg_database." user=".$pg_user." password=".$pg_pass;
  }

  function getDefaultSet(){
    global $default_set;
    return $default_set;
  }

  function getChartArray($chartId){
    global $cImg;
    return $cImg[$chartId];
  }
 
  function getDefaultChartDir(){
    global $default_chart_dir;
    return $default_chart_dir."/";
  }
  
  function getRptArray($rptId){
    global $report;
    return $report[$rptId];
  }

  function getAllTitle(){
    global $allTitle;
    return $allTitle;
  }
}
?>
