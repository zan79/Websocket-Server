/*
 * Websocket Project for DC105
 * 
 * Group Members:
 * Bryle Anthony Orevillo
 * Glenn Gerald Dapitilla
 * Melchizedek Cruel
 * Lord Son Pabriga
 * R.A. Patricia Corage
 * 
 * Hardware used:
 * 
 * NodeMCU ESP32 Dev Module
 * WS2812b LED 16leds 
 * 
*/

var url = "ws://192.168.4.1:1337/";
var toggleButton;
var redButton;
var greenButton;
var blueButton;

function init() {
    toggleButton = document.getElementById("toggleButton");
    redButton = document.getElementById("redButton");
    greenButton = document.getElementById("greenButton");
    blueButton = document.getElementById("blueButton");

    wsConnect(url);
}

function wsConnect(url) {
    websocket = new WebSocket(url);

    websocket.onopen = function (evt) { onOpen(evt) };
    websocket.onclose = function (evt) { onClose(evt) };
    websocket.onmessage = function (evt) { onMessage(evt) };
    websocket.onerror = function (evt) { onError(evt) };
}

function onOpen(evt) {

    console.log("Connected");

    toggleButton.disabled = false;
    redButton.disabled = false;
    greenButton.disabled = false;
    blueButton.disabled = false;

    doSend("getLEDState");
}

function onClose(evt) {

    console.log("Disconnected");

    toggleButton.disabled = true;
    redButton.disabled = true;
    greenButton.disabled = true;
    blueButton.disabled = true;

    setTimeout(function () { wsConnect(url) }, 2000);
}

function onMessage(evt) {

    console.log("Received: " + evt.data);

    switch (evt.data) {
        case "0":
            console.log("LED is off");
            document.getElementById("led-light").style.backgroundColor = "black";
            toggleButton.style.backgroundColor = "gray";
            toggleButton.innerText = 'OFF';
            break;
        case "1":
            console.log("LED is on");
            document.getElementById("led-light").style.backgroundColor = "red";
            toggleButton.style.backgroundColor = "white";
            toggleButton.innerText = 'ON';
            break;
        case "2":
            console.log("LED is on");
            document.getElementById("led-light").style.backgroundColor = "green";
            toggleButton.style.backgroundColor = "white";
            toggleButton.innerText = 'ON';
            break;
        case "3":
            console.log("LED is on");
            document.getElementById("led-light").style.backgroundColor = "blue";
            toggleButton.style.backgroundColor = "white";
            toggleButton.innerText = 'ON';
            break;
        default:
            break;
    }
}

function onError(evt) {
    console.log("ERROR: " + evt.data);
}

function doSend(message) {
    console.log("Sending: " + message);
    websocket.send(message);
}

function onToggle() {
    doSend("toggleLED");
    doSend("getLEDState");
}
function onPressRed() {
    doSend("ledRED");
    doSend("getLEDState");
}
function onPressGreen() {
    doSend("ledGREEN");
    doSend("getLEDState");
}
function onPressBlue() {
    doSend("ledBLUE");
    doSend("getLEDState");
}

window.addEventListener("load", init, false);