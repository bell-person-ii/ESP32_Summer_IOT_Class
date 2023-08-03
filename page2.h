const char page2[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
  <meta name= 'viewport' content= 'width=device-width, initial-scale=1.0'>
  <style>
  .wb       { margin: 30px auto; padding: 15px; display: flex; flex-direction: column; background: white;}
  .la1      { margin: 10px auto; padding: 10px 10px; display:inline; font-size: 20px; width: 100px;text-align: right; }
  .in1      { margin: 10px auto; padding: 10px 10px; display:inline; font-size: 20px; width: 100px;text-align: left;  }
  .sw_bt    { font-size: 20px;   padding: 10px 25px; border-radius: 5px; background: black; color: white;}
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
       <h1 style='align-self: center;'>Switch Operation</h1>      
       <div style="align-self: center;margin-top: 25px;" id='grid'>
         <button id='sw1' class ='sw_bt'>SW1</button>
       </div>
       <div style="align-self: center;margin-top: 25px;" id='grid'>
       <a href="/" conclick = "stopColor()" class="prev round">&#8250;</a>
       </div>
   </div>  
  </body>

 <script>
  const sw = document.getElementById('sw1');
  document.addEventListener('DOMContentLoaded', () => {
    sw.addEventListener('click', function(){
      fetch('sw1', {
         method: 'POST',
         headers: { 'Content-Type': 'application/json' },
         body: JSON.stringify({
           sw1:sw.value,
            })
         })
      .then(response => response.json())
      .then(response => { sw.value = response['sw1']})
      .then(result   => { (sw.value === "0" ) ? sw.value = "1" : sw.value = "0" })
    })
 }) 
document.addEventListener('DOMContentLoaded', async function () {
  let response = await fetch('/sw1Get');
  let result   = await response.json();
  result['sw1'] === "0" ? sw.value = "1" : sw.value = "0" ;
})
var myVar = setInterval(setColor,300);
function setColor() {
  if     (sw.value === "1" ) sw.style.backgroundColor = "black";
  else if(sw.value === "0" ) sw.style.backgroundColor = "red";
}
function stopColor() {
  clearInterval(myVar);
}

    
</script>
</html>
)=====";
