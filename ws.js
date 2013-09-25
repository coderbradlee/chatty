
ws = new WebSocket('ws://localhost:50000');
ws.onopen = function() {
    console.log('open');
}
ws.onerror = function() {
    console.log('error');
}
ws.onclose = function() {
    console.log('close');
}
