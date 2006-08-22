<?
/* Datenbankverbindung */
  $pg_host 	= "141.30.228.39"; 
  $pg_database 	= "wetter";
  $pg_user 	= "losinshi";
  $pg_pass 	= "";

/* Default-Werte */
  $default_set	= "test";

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

}
?>
