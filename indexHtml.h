#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML>
    <html>
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
            body {
                font-family: Arial;
                text-align: center;
                margin: 0px auto;
                padding-top: 30px;
            }
            .button {
                padding: 15px 30px;
                font-size: 20px;
                color: white;
                background-color: #2f4468;
                border: none;
                border-radius: 5px;
                cursor: pointer;
            }
            .button:hover {
                background-color: #1f2e45;
            }
        </style>
    </head>
    <body>
        <h1>ESP Web Controller</h1>
        <button class="button" onclick="toggleAlgorithm();">Toggle Algorithm</button>

        <script>
            function toggleAlgorithm() {
                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/toggle", true);
                xhr.send();
                xhr.onload = function() {
                    console.log("Server response: " + xhr.responseText);
                }
            }
        </script>
    </body>
    </html>
)rawliteral";

#endif
