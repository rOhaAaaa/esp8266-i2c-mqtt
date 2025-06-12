#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; padding-top: 30px; }
    .button {
      padding: 10px 20px;
      font-size: 18px;
      color: white;
      background-color: #2f4468;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      margin: 5px;
    }
    .button:hover { background-color: #1f2e45; }
    .grid {
      display: flex;
      justify-content: center;
      margin-top: 20px;
    }
    .box {
      width: 100px;
      height: 100px;
      margin: 10px;
      border: 3px solid green;
      background-color: white;
    }
  </style>
</head>
<body>
  <h1>ESP Pushbutton Web Server</h1>
  <div>
    <button class="button" onclick="toggleAlgorithm1();" id="alg1Btn">Start Algorithm 1</button>
    <button class="button" onclick="toggleAlgorithm2();" id="esp2Btn">Start Algorithm 2</button>
  </div>
  <div class="grid">
    <div id="box1" class="box"></div>
    <div id="box2" class="box"></div>
    <div id="box3" class="box"></div>
  </div>

  <script>
    let alg1Running = false;
    let alg2Running = false;

    function toggleAlgorithm1() {
      fetch('/toggle')
        .then(response => response.text())
        .then(state => {
          alg1Running = state === "ON";
          document.getElementById("alg1Btn").innerText = alg1Running ? "Stop Algorithm 1" : "Start Algorithm 1";
        });
    }

    function toggleAlgorithm2() {
      const action = alg2Running ? 'S' : 'P';
      fetch(`/esp2?cmd=${action}`)
        .then(response => response.text())
        .then(text => {
          alg2Running = !alg2Running;
          document.getElementById("esp2Btn").innerText = alg2Running ? "Stop Algorithm 2" : "Start Algorithm 2";
        });
    }

    function updateBoxes(state) {
      const boxes = ["box1", "box2", "box3"];
      const colors = ["red", "orange", "green"];
      boxes.forEach((box, index) => {
        document.getElementById(box).style.backgroundColor = state === index ? colors[index] : "white";
      });
    }

    setInterval(() => {
      fetch('/state')
        .then(response => response.text())
        .then(state => updateBoxes(parseInt(state)))
        .catch(error => console.log('Error:', error));
    }, 500);
  </script>
</body>
</html>
)rawliteral";

#endif
