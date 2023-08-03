const char page1[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
  <meta name= 'viewport' content= 'width=device-width, initial-scale=1.0'>
  <style>
  .wb       { margin: 30px auto; padding: 15px; display: flex; flex-direction: column; background: white;}
  .la1      { margin: 10px auto; padding: 10px 10px; display:inline; font-size: 20px; width: 100px;text-align: right; }
  .in1      { margin: 10px auto; padding: 10px 10px; display:inline; font-size: 20px; width: 100px;text-align: left;  }
  .udate_bt { font-size: 20px;   padding: 10px 25px; border-radius: 5px; background: green; color: white;}
  .reset_bt { font-size: 20px;   padding: 10px 25px; border-radius: 5px; background: red;   color: white;}
  a         { text-decoration: none; display: inline-block; padding: 8px 16px; }
  a:hover   { background-color: #ddd;    color: black; }
  .prev     { background-color: #f1f1f1; color: black; }
  .next     { background-color: #4CAF50; color: white; }
  .round    { border-radius: 50%;
  </style>
    <title>myiot local</title>
  </head>
  <body style='background: steelblue;'>
    <div class = 'wb'>
         <h1 style='align-self: center;'>AP mode setting</h1>
         <div style="align-self: center;margin-top: 25px;" id='grid''>    
         <div>
           <label for='ssid' class ='la1' >SSID:</label>
           <input class ='in1' type='text' name='ssid' id='ssid'  value>
         </div>
         <div>
           <label for='pass' class ='la1' >PASS:</label>
           <input class ='in1' type='text' name='pass' id='pass'  value>
         </div>
       <div style="align-self: center;margin-top: 50px;" id='grid'>
       </div>
       <div style="align-self: center;margin-top: 25px;" id='grid'>
         <button id='update' class ='udate_bt'>UPDATE</button>
         <button id='reset'  class ='reset_bt'>RESET</button>
       </div>
       <div style="align-self: center;margin-top: 25px;" id='grid'>
       <a href="/next" class="next round">&#8250;</a>
       </div>
   </div>  
  </body>
  <script>
  document.addEventListener('DOMContentLoaded', () => {
    document.getElementById('update').addEventListener('click', function(){
      fetch('update', {
         method: 'POST',
         headers: { 'Content-Type': 'application/json' },
         body: JSON.stringify({
           ssid:document.getElementById('ssid').value,
           pass:document.getElementById('pass').value,
            })
         })
      .then(response => response.json())
      .then(response => { document.getElementById('ssid').value = response['ssid'],
                          document.getElementById('pass').value = response['pass'], 
                          alert(JSON.stringify(response))})
   }),
   document.getElementById('reset').addEventListener('click', function(){  
      fetch('reset', {
         method: 'POST',
         headers: { 'Content-Type': 'application/json' },
         body: JSON.stringify({
           reset:document.getElementById('reset').value,
         })
      })
      .then(response => response.json()) 
   })   
 })
 
 document.addEventListener('DOMContentLoaded', async function () {
  const ssid = document.getElementById('ssid')
  const pass = document.getElementById('pass')
  let response = await fetch('/get');
  let result =   await response.json();
  ssid.value = result['ssid'];
  pass.value = result['pass'];
})
</script>
</html>
)=====";
