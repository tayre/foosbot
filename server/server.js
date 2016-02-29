var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  socket.on('input_message', function(msg){
    console.log('incoming input_message was: ' + msg);
    io.emit('broadcast_message', msg);
  });
});

var port = 3000;
http.listen(port, function(){
  console.log('listening on localhost:' + port);
});


// tcp server
var net = require('net');

net.createServer(function (socket) {

  io.emit('broadcast_message', 'client connected ' + new Date());

  socket.on('data', function(data) {
    var line = data.toString();
    console.log('got data', line);
    io.emit('broadcast_message', line); // send to all clients
  });

  socket.on('end', function() {
    console.log('connection end');
    io.emit('broadcast_message', 'connection end');
  });

  socket.on('close', function() {
    console.log('connection close');
    io.emit('broadcast_message', 'connection close');
  });

  socket.on('error', function(e) {
    console.log('error ', e);
    io.emit('broadcast_message', e);
  });

  socket.write("hello from tom's tcp server");
}).listen(3001, function() {
  console.log('TCP Server is listening on port 3001');
});
