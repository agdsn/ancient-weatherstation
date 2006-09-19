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
/* Bild - Id				Dateiname		Link - Name		Link - Beschreibung								Vorschaubild	Set */
  'temp_10_1'		=> array(	"temp_ex.png", 		"Liniendiagram",	"Zeigt den Temparaturverlauf der letzten 10 Tage in einem Diagram", 		null,		"chart"),
  'temp_30_1'		=> array(	"temp_ex.png", 		"Liniendiagram",	"Zeigt den Temparaturverlauf der letzten 30 Tage in einem Diagram", 		null,		"chart"),
  'temp_365_1'		=> array(	"temp_ex.png", 		"Liniendiagram",	"Zeigt den Temparaturverlauf des letzten Jahres in einem Diagram", 		null,		"chart"),

  'hum_10_1'		=> array(	"hum_ex.png", 		"Liniendiagram",	"Zeigt den Verlauf der Luftfeuchtigkeit in den letzten 10 Tagen als Diagram",	null, 		"chart"),
  'hum_30_1'		=> array(	"hum_ex.png", 		"Liniendiagram",	"Zeigt den Verlauf der Luftfeuchtigkeit in den letzten 30 Tagen als Diagram",	null, 		"chart"),
  'hum_365_1'		=> array(	"hum_ex.png", 		"Liniendiagram",	"Zeigt den Verlauf der Luftfeuchtigkeit im letztem Jahr als Diagram",		null, 		"chart"),

  'press_10_1'		=> array(	"press_ex.png", 	"Liniendiagram",	"Zeigt den Verlauf des Luftdrucks in den letzten 10 Tagen als Diagram",		null, 		"chart"),
  'press_30_1'		=> array(	"press_ex.png", 	"Liniendiagram",	"Zeigt den Verlauf des Luftdrucks in den letzten 30 Tagen als Diagram", 	null, 		"chart"),
  'press_365_1'		=> array(	"press_ex.png", 	"Liniendiagram",	"Zeigt den Verlauf des Luftdrucks im letztem Jahr als Diagram", 		null, 		"chart"),

  'rain_10_3'		=> array(	"rain_ex.png", 		"Balkendiagram",	"Zeigt die Niederschlagsmengen der letzten 10 Tage als Diagram",		null, 		"chart"),
  'rain_30_3'		=> array(	"rain_ex.png", 		"Balkendiagram",	"Zeigt die Niederschlagsmengen der letzten 30 Tage als Diagram", 		null, 		"chart"),
  'rain_365_3'		=> array(	"rain_ex.png", 		"Balkendiagram",	"Zeigt die Niederschlagsmengen des letzten Jahres als Diagram", 		null, 		"chart"),

  'wind_10_2'		=> array(	"wind_ex.png", 		"Liniendiagram",	"Zeigt den Verlauf der Windgeschwindigkeit in den letzten 10 Tagen als Diagram",null, 		"chart"),
  'wind_30_2'		=> array(	"wind_ex.png", 		"Liniendiagram",	"Zeigt den Verlauf der Windgeschwindigkeit in den letzten 30 Tagen als Diagram",null, 		"chart"),
  'wind_365_2'		=> array(	"wind_ex.png", 		"Liniendiagram",	"Zeigt den Verlauf der Windgeschwindigkeit im letzten Jahr als Diagram",	null, 		"chart"),

  'temp_10_4'		=> array(	"temp_ex.png", 		"Liniendiagram",	"Zeigt den Temparaturverlauf der letzten 10 Tage in einem Diagram", 		null,		"chart"),
  'temp_30_4'		=> array(	"temp_ex.png", 		"Liniendiagram",	"Zeigt den Temparaturverlauf der letzten 30 Tage in einem Diagram", 		null,		"chart"),
  'temp_365_4'		=> array(	"temp_ex.png", 		"Liniendiagram",	"Zeigt den Temparaturverlauf des letzten Jahres in einem Diagram", 		null,		"chart"),

  'hum_10_4'		=> array(	"hum_ex.png", 		"Liniendiagram",	"Zeigt den Verlauf der Luftfeuchtigkeit in den letzten 10 Tagen als Diagram",	null, 		"chart"),
  'hum_30_4'		=> array(	"hum_ex.png", 		"Liniendiagram",	"Zeigt den Verlauf der Luftfeuchtigkeit in den letzten 30 Tagen als Diagram",	null, 		"chart"),
  'hum_365_4'		=> array(	"hum_ex.png", 		"Liniendiagram",	"Zeigt den Verlauf der Luftfeuchtigkeit im letztem Jahr als Diagram",		null, 		"chart"),
  '' 			=> array(	"", 			"", 			"",										null, 		"chart")
);

$report = array(
/* Report-Id				Link - Name 		Link - Beschreibung								sensId	TabellenFeld	Interval	Anz.Interval	FeldBezeichn		Fakt.	Unit						Mod_name 	Mittelwert	DatumsFormat		ModuleSet*/		
  'temp_10_1'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Temparatur - Werte der Letzten 10 Tage",		1,	"temp",		"hour",		"10 days", 	"Temparatur",		0.1,	"�C",						"temp", 	true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'temp_30_1'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Temparatur - Werte der Letzten 30 Tage",		1,	"temp",		"hour",		"30 days", 	"Temparatur",		0.1,	"�C",						"temp", 	true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'temp_365_1'		=>array(	"Daten - Report",	"Listet T&auml;gliche Temparatur - Werte des letzten Jahres",			1,	"temp",		"day",		"1 years", 	"Temparatur",		0.1,	"�C",						"temp", 	true,		"DD.MM.YYYY ",				"report"),

  'hum_10_1'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Luftfeuchtigkeits - Werte der Letzten 10 Tage",		1,	"hum",		"hour",		"10 days", 	"Luftfeuchtigkeit",	1,	"%",						"hum", 		true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'hum_30_1'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Luftfeuchtigkeits - Werte der Letzten 10 Tage",		1,	"hum",		"hour",		"30 days", 	"Luftfeuchtigkeit",	1,	"%",						"hum", 		true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'hum_365_1'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Luftfeuchtigkeits - Werte der Letzten 10 Tage",		1,	"hum",		"day",		"1 years", 	"Luftfeuchtigkeit",	1,	"%",						"hum", 		true,		"DD.MM.YYYY ",				"report"),

  'press_10_1'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Luftdruck - Werte der Letzten 10 Tage",			1,	"press",	"hour",		"10 days", 	"Luftdruck",		1,	"hPa",						"press", 	true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'press_30_1'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Luftdruck - Werte der Letzten 10 Tage",			1,	"press",	"hour",		"30 days", 	"Luftdruck",		1,	"hPa",						"press", 	true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'press_365_1'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Luftdruck - Werte der Letzten 10 Tage",			1,	"press",	"day",		"1 years", 	"Luftdruck",		1,	"hPa",						"press", 	true,		"DD.MM.YYYY ",				"report"),

  'wind_10_2'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Windgeschwindigkeits - Werte der Letzten 10 Tage",	2,	"geschw",	"hour",		"10 days", 	"Windgeschwindigkeit",	0.1,	"<sup>km</sup>/<sub>h</sub>",			"wind", 	true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'wind_30_2'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Windgeschwindigkeits - Werte der Letzten 10 Tage",	2,	"geschw",	"hour",		"30 days", 	"Windgeschwindigkeit",	0.1,	"<sup>km</sup>/<sub>h</sub>",			"wind", 	true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'wind_365_2'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Windgeschwindigkeits - Werte der Letzten 10 Tage",	2,	"geschw",	"day",		"1 years", 	"Windgeschwindigkeit",	0.1,	"<sup>km</sup>/<sub>h</sub>",			"wind", 	true,		"DD.MM.YYYY ",				"report"),

  'rain_10_3'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Niederschlags - Werte der Letzten 10 Tage",		3,	"count",	"hour",		"10 days", 	"Niederschlag",		0.001,	"<sup>l</sup>/<sub>m<sup>2</sup></sub>",	"rain", 	false,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'rain_30_3'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Niederschlags - Werte der Letzten 10 Tage",		3,	"count",	"hour",		"30 days", 	"Niederschlag",		0.001,	"<sup>l</sup>/<sub>m<sup>2</sup></sub>",	"rain", 	false,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'rain_365_3'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Niederschlags - Werte der Letzten 10 Tage",		3,	"count",	"day",		"1 years", 	"Niederschlag",		0.001,	"<sup>l</sup>/<sub>m<sup>2</sup></sub>",	"rain", 	false,		"DD.MM.YYYY ",				"report"),

  'temp_10_4'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Temparatur - Werte der Letzten 10 Tage",		4,	"temp",		"hour",		"10 days", 	"Temparatur",		0.1,	"�C",						"temp", 	true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'temp_30_4'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Temparatur - Werte der Letzten 30 Tage",		4,	"temp",		"hour",		"30 days", 	"Temparatur",		0.1,	"�C",						"temp", 	true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'temp_365_4'		=>array(	"Daten - Report",	"Listet T&auml;gliche Temparatur - Werte des letzten Jahres",			4,	"temp",		"day",		"1 years", 	"Temparatur",		0.1,	"�C",						"temp", 	true,		"DD.MM.YYYY ",				"report"),

  'hum_10_4'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Luftfeuchtigkeits - Werte der Letzten 10 Tage",		4,	"hum",		"hour",		"10 days", 	"Luftfeuchtigkeit",	1,	"%",						"hum", 		true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'hum_30_4'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Luftfeuchtigkeits - Werte der Letzten 10 Tage",		4,	"hum",		"hour",		"30 days", 	"Luftfeuchtigkeit",	1,	"%",						"hum", 		true,		"DD.MM.YYYY &nbsp;&nbsp; HH24:MI",	"report"),
  'hum_365_4'		=>array(	"Daten - Report",	"Listet St&uuml;ndliche Luftfeuchtigkeits - Werte der Letzten 10 Tage",		4,	"hum",		"day",		"1 years", 	"Luftfeuchtigkeit",	1,	"%",						"hum", 		true,		"DD.MM.YYYY ",				"report")
);





/* Config-Klasse, Bitte nicht �ndern! */
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
  
  function getRptArray($rptId){
    global $report;
    return $report[$rptId];
  }
}
?>
