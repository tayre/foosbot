var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/', function(req, res){
  console.log('requested main file');
  res.sendfile('index.html');
});

app.get('/hi', function(req, res) {
  console.log('hi received');
  res.send('hi back');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.emit('increment protocol', '2');

  socket.on('increment protocol', function(msg){
    console.log('message: ' + msg);
    socket.emit('increment protocol', msg);
  });
});

var server = http.listen(80, function(){
  console.log('listening on *:80');
});

// this function is called when you want the server to die gracefully
// i.e. wait for existing connections
var gracefulShutdown = function() {
  console.log("Received kill signal, shutting down gracefully.");
  server.close(function() {
    console.log("Closed out remaining connections.");
    process.exit()
  });

  setTimeout(function() {
    console.error("Could not close connections in time, forcefully shutting down.");
    process.exit()
  }, 2000);
}

// listen for TERM signal .e.g. kill
process.on ('SIGTERM', gracefulShutdown);

// listen for INT signal e.g. Ctrl-C
process.on ('SIGINT', gracefulShutdown);
