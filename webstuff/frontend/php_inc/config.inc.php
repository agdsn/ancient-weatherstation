<?
/* Datenbankverbindung */
  $pg_host 	= "141.30.228.39"; 
  $pg_database 	= "wetter";
  $pg_user 	= "losinshi";
  $pg_pass 	= "";

/* Default-Werte */
  $default_set	     = "test";
  $default_chart_dir = "images/chart/";



/* Graphen - Bilder */
$cImg = array(
/* Bild - Id				Dateiname		Link - Name		Link - Beschreibung					Vorschaubild	Set */
  'temp_test_1'		=> array(	"temp_ex.png", 		"Beispiel - Temp",	"Zeigt den Temparaturverlauf der letzten 10 Tage", 	null,		"chart"),
  'hum_test_1'		=> array(	"hum_ex.png", 		"Beispiel - Hum",	"Zeigt den Luftfeuchte - verlauf der letzten 10 Tage",	null, 		"chart"),
  'press_test_1'	=> array(	"press_ex.png", 	"Beispiel - Press",	"Zeigt den Luftdruck - verlauf der letzten 10 Tage", 	null, 		"chart"),
  '' 			=> array(	"", 			"", 			"",							null, 		"chart")
);






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
    return $default_chart_dir;
  }
}
?>
