<?

  /* copyright: Jan Losinski, 2006
     
     at the Moment this is no free software, look at the
     COPYING-File in the Main-Directory for License-Terms
  */

include_once($path."php_inc/config.inc.php");

/* Connection-Klasse..ist fuer die Datenbankverbindung zustaendig */
class Cacher{

  function _cacheFileName($identifier){
    $tmp_dir = Config::getCachePath();
    if(!file_exists($tmp_dir)){
      mkdir(dirname($tmp_dir), 0700);
      mkdir($tmp_dir, 0700);
    }
    return $tmp_dir.md5($identifier);
  }
  function _lockFileName($identifier){
    return Cacher::_cacheFileName($identifier).".lock";
  }

  function _checkCache($identifier, $expire){
    $file = Cacher::_cacheFileName($identifier);
    if (file_exists($file)){
      if (filemtime($file) > (time() - ($expire *60))){
	return true;
      }
    }
    return false;
  }

  function _lockRead($identifier){
    $handle = fopen(Cacher::_lockFileName($identifier), "w");
    $i = 0;
    while (flock($handle, LOCK_SH) != true){
      usleep(10);
      if ($i > 10){
	fclose($handle);
    echo "bla1";
	return false;
      }
      $i ++;
    }
    return $handle;
  }


  function _unlockRead($handle){
    flock($handle, LOCK_UN);
    fclose($handle);
  }


  function _lockWrite($identifier){
    $handle = fopen(Cacher::_lockFileName($identifier), "a+");
    $i = 0;
    while (flock($handle, LOCK_EX) != true){
      usleep(10);
      if ($i > 10){
	fclose($handle);
    echo "bla";
	return false;
      }
      $i++;
    }
    return $handle;
  }


  function _unlockWrite($handle){
    flock($handle, LOCK_UN);
    fclose($handle);
  }

  function getCache($identifier, $expire){
    if(!Config::performCaching()){
      return false;
    }
    $lock = null;
    if (Cacher::_checkCache($identifier, $expire) && (($lock = Cacher::_lockRead($identifier)) != false)){
      $file = Cacher::_cacheFileName($identifier);
      $handle = fopen($file, "r");
      $serialized = fread ($handle, filesize ($file));
      Cacher::_unlockRead($lock);
      return unserialize($serialized);
    } else {
      return false;
    }
  }
  function setCache($identifier, $var){
    if(!Config::performCaching()){
      return false;
    }
    $lock = null;
    if (($lock =  Cacher::_lockWrite($identifier)) != false){
      $file = Cacher::_cacheFileName($identifier);
      $handle = fopen($file, "w");
      fwrite($handle, serialize($var));
      fclose($handle);
      Cacher::_unlockWrite($lock);
    }
  }


}
