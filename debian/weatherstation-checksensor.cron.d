#
# Regular cron jobs for the weatherstation package
#
0 12 	* * * 	weatherstation	/usr/bin/weather_checksensor 2>&1 >/dev/null
