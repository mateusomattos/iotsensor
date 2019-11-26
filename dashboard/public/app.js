
(function(){
    
  // Inicia o firebase Firebase
  var config = {
    apiKey: "AIzaSyBlziCO-ju8Tty2ydzVu-354vdIuENq7Os",
    authDomain: "iotsensor-cc9bd.firebaseapp.com",
    databaseURL: "https://iotsensor-cc9bd.firebaseio.com",
    storageBucket: "iotsensor-cc9bd.appspot.com",
    messagingSenderId: "999139819416"
  };
  firebase.initializeApp(config);

  var db = firebase.database();



  // Cria os listeners dos dados no firebase
  var tempRef = db.ref('temperature');
  var umidRef = db.ref('humidity');
  var tempRef2 = db.ref('temperature2');
  var umidRef2 = db.ref('humidity2');
  var maxRef = db.ref('maxTemp');
  var minRef = db.ref('minTemp');



  // Registra as funções que atualizam os gráficos e dados atuais da telemetria
  tempRef.on('value', onNewData('currentTemp', 'tempLineChart' , 'Temperatura', 'ºC'));
  umidRef.on('value', onNewData('currentUmid', 'umidLineChart' , 'Umidade', '%'));
  tempRef2.on('value', onNewData('currentTemp2', 'tempLineChart2' , 'Temperatura', 'ºC'));
  umidRef2.on('value', onNewData('currentUmid2', 'umidLineChart2' , 'Umidade', '%'));
  maxRef.on('value', onNewData('maxTemp', 'maxTempChart' , 'Temperatura', 'ºC'));
  minRef.on('value', onNewData('minTemp', 'minTempChart' , 'Temperatura', 'ºC'));



})();

// Retorna uma função que de acordo com as mudanças dos dados
// Atualiza o valor atual do elemento, com a metrica passada (currentValueEl e metric)
// e monta o gráfico com os dados e descrição do tipo de dados (chartEl, label)

function arrayAverage(arr){
    //Find the sum
    var sum = 0;
    for(var i in arr) {
        sum += arr[i];
    }
    //Get the length of the array
    var numbersCnt = arr.length;
    //Return the average / mean.
    return (sum / numbersCnt);
}

function fillArray(value, len) {
  if (len == 0) return [];
  var a = [value];
  while (a.length * 2 <= len) a = a.concat(a);
  if (a.length < len) a = a.concat(a.slice(0, len - a.length));
  return a;
}


function onNewData(currentValueEl, chartEl, label, metric){
  return function(snapshot){
    var readings = snapshot.val();
    if(readings){
        var currentValue;
        var data = [];
        var len = 13;
        var keys = Object.keys(readings);
        for(var i = 0; i < keys.length; i++){
          var k = keys[i];
          currentValue = readings[k]
          data.push(currentValue);
        }

        document.getElementById(currentValueEl).innerText = currentValue + ' ' + metric;
        var lenKey = keys.length;

        var rootRef = firebase.database().ref("time");
        rootRef.once("value")
          .then(function(snapshot) {
            var value = snapshot.val();
            var keyTime = Object.keys(value);
            var time = [];
            //console.log(keyTime);
            for(var j = 0; j <keyTime.length; j++){
              k = keyTime[j];
              var currentTime = value[k];
              time.push(currentTime);
            }

                    if(keys.length > 1){
          buildLineChart(chartEl, label, data.slice(-13), time.slice(-13));
        }

        if(keys.length == 1) {
          buildLineChart(chartEl, label, data, time.slice(-13));
          document.getElementById(currentValueEl).innerText = currentValue + ' ' + metric;
        }
          });
        mean = arrayAverage(data);
        var meanArray = fillArray(mean, len);
    }
  }
}


// Constroi um gráfico de linha no elemento (el) com a descrição (label) e os
// dados passados (data)
function buildLineChart(el, label, data, time){
  var elNode = document.getElementById(el);

  new Chart(elNode, {
    type: 'line',

    data: {
        labels: time,
        datasets: [{
            label: label,
            data: data,
            borderWidth: 1,
            fill: false,
            spanGaps: false,
            lineTension: 0.1,
            backgroundColor: "#F9A825",
            borderColor: "#F9A825"
        }]
    }, 

    options: {
      scales: {
        xAxes: [{
          scaleLabel: {
            display: true,
            labelString: 'Horas atrás'
          }
        }]
      }     
    }
  });

  $(document).ready(function(){
    $('.tabs').tabs();
  });
}