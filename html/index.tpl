<html><head><title>THUNERSTORM PROTECTED IOT SWITCH</title>
<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>			
<div id="main">
<center><h1>THUNERSTORM PROTECTED IOT SWITCH</h1></center>


<h2>
<center>User Settings </center>
</h2>
<p>
Distance threshold %distance_threshold%  <form method="post" action="as3935.cgi" oninput="x.value=parseInt(distance.value)">1 <input type="range"  name="distance"  min="1" max="40" value="%distance_threshold%"> 40 km <output name="x" for="distance"></output> <input type="submit"></form>		
</p>
<p>
Timeout; %timeout_threshold% <form method="post" action="as3935.cgi" oninput="x.value=parseInt(timeout.value)">1 <input type="range"  name="timeout"  min="1" max="120" value="%timeout_threshold%"> 120 minutes  <output name="x" for="timeout"></output> <input type="submit"></form>				
</p>
<p>
Relay state;	%relay_state%  
<form method="post" action="as3935.cgi">
<input type="submit" name="state" value=0>
<input type="submit" name="state" value=1>
<input type="submit" name="state" value=2>
<input type="submit" name="state" value=3>
</form>
</p>
<center><h2>AS3935 thuderstike sensor settings and fine tunning </h2></center>
<p>
AFE_GB - analog front end gain boost: %AFE_GB%  
	
<form method="post" action="as3935.cgi">	
<input type="radio" name="boost" value=18>Indoor 
<input type="radio" name="boost" value=14>Outdoor
<input type="submit">
</form>
</p>
<p> 
NF_LEV -noise floor level: %NF_LEV%
	
<form method="post" action="as3935.cgi">
<input type="submit" name="noise" value=0>
<input type="submit" name="noise" value=1>
<input type="submit" name="noise" value=2>
<input type="submit" name="noise" value=3>
<input type="submit" name="noise" value=4>
<input type="submit" name="noise" value=5>
<input type="submit" name="noise" value=6>
<input type="submit" name="noise" value=7>
</form>
</p>
<p>
WDTH - watchdog threshold: %WDTH%

<form method="post" action="as3935.cgi">
<input type="submit" name="wdth" value=0>
<input type="submit" name="wdth" value=1>
<input type="submit" name="wdth" value=2>
<input type="submit" name="wdth" value=3>
<input type="submit" name="wdth" value=4>
<input type="submit" name="wdth" value=5>
<input type="submit" name="wdth" value=6>
<input type="submit" name="wdth" value=7>
<input type="submit" name="wdth" value=8>
<input type="submit" name="wdth" value=9>
<input type="submit" name="wdth" value=10>
<input type="submit" name="wdth" value=11>
<input type="submit" name="wdth" value=12>
<input type="submit" name="wdth" value=13>					
<input type="submit" name="wdth" value=14>
</form>			
</p>
<p>
CL_STAT - clear statistic
<form method="post" action="as3935.cgi">
<input type="submit" name="clear" value=1>
</form>
</p> 
<p>			
MIN_NUM_LIGH, minimum number of lightnings: %MIN_NUM_LIGH%
</p>
<p>
<form method="post" action="as3935.cgi">
<input type="submit" name="min" value=1>
<input type="submit" name="min" value=5>						
<input type="submit" name="min" value=9>
<input type="submit" name="min" value=15>
</form>
</p> 
<p>
SREJ - Spike Rejection:	%SREJ%	
</p>
<p>
<form method="post" action="as3935.cgi">
<input type="submit" name="spike" value=0>
<input type="submit" name="spike" value=1>						
<input type="submit" name="spike" value=2>
<input type="submit" name="spike" value=3>
<input type="submit" name="spike" value=4>
<input type="submit" name="spike" value=5>
<input type="submit" name="spike" value=6>
<input type="submit" name="spike" value=7>	
<input type="submit" name="spike" value=8>
<input type="submit" name="spike" value=9>
<input type="submit" name="spike" value=10>
<input type="submit" name="spike" value=11>	
<input type="submit" name="spike" value=12>
<input type="submit" name="spike" value=13>
<input type="submit" name="spike" value=14>
<input type="submit" name="spike" value=15>	
</form>			
</p> 
<p>
DISTANCE: %DISTANCE%
</p> 
Calibration(will turn relay device off) calibration steps: <button type="button" onclick="alert('enable DISP_LCO by seting Relay state to 2 (outputs lco resonant frequency to irq_pin)')">1</button>
<button type="button" onclick="alert('use osciloscope or frequency counter to probe irq_pin, and adjusting TUN_CAP so that you will get as close as possible to 31.25khz')">2</button>
<br>
</p> 
<p>
TUN_CAP - internal tunning variable capacitor: %TUN_CAP% &micro;F
</p>  
<p>
<form method="post" action="as3935.cgi">
<input type="submit" name="cap" value=0>
<input type="submit" name="cap" value=8>
<input type="submit" name="cap" value=16>
<input type="submit" name="cap" value=24>
<input type="submit" name="cap" value=32>
<input type="submit" name="cap" value=40>
<input type="submit" name="cap" value=48>
<input type="submit" name="cap" value=56>
<input type="submit" name="cap" value=64>	
<input type="submit" name="cap" value=72>
<input type="submit" name="cap" value=80>
<input type="submit" name="cap" value=88>	
<input type="submit" name="cap" value=96>
<input type="submit" name="cap" value=104>	
<input type="submit" name="cap" value=112>
<input type="submit" name="cap" value=120>	
</form>
</p>

<p><a href="/wifi">Wifi settings</a></p>
<p>
<center>&copy; 2015 golob.uros@gmail.com </center>
</p>
</div>
</body>
</html>
