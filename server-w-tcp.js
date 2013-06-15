var http = require('http')
  , net = require('net')
  , url = require('url')
  , fs = require('fs')
  , io = require('socket.io')
  , sys = require(process.binding('natives').util ? 'util' : 'sys')
  , server;
  
var tcpGuests = [];
var chatGuests = [];

server = http.createServer(function(req, res){
  // your normal server code
  var path = url.parse(req.url).pathname;
  switch (path){
		case '/js/jquery.gracefulwebsocket.js':
			fs.readFile(__dirname + path, function(err, data){
        if (err) return send404(res);
        res.writeHead(200, { 'Content-Type' : 'text/javascript' });
        res.write(data, 'utf8');
        res.end();
      });
      break;
		case '/js/jquery.js':
			fs.readFile(__dirname + '/js/jquery.js', function(err, data){
        if (err) return send404(res);
        res.writeHead(200, { 'Content-Type' : 'text/javascript' });
        res.write(data, 'utf8');
        res.end();
      });
      break;
		case '/js/jquery.mobile.custom.js':
			fs.readFile(__dirname + path, function(err, data){
        if (err) return send404(res);
        res.writeHead(200, { 'Content-Type' : 'text/javascript' });
        res.write(data, 'utf8');
        res.end();
      });
      break;

		case '/js/jquery-ui.custom.min.js':
			fs.readFile(__dirname + path, function(err, data){
        if (err) return send404(res);
        res.writeHead(200, { 'Content-Type' : 'text/javascript' });
        res.write(data, 'utf8');
        res.end();
      });
      break;
		case '/js/jquery.mobile.custom.structure.css':
			fs.readFile(__dirname + path, function(err, data){
        if (err) return send404(res);
        res.writeHead(200, { 'Content-Type' : 'text/css' });
        res.write(data, 'utf8');
        res.end();
      });
      break;
		case '/js/jquery.mobile.custom.theme.css':
			fs.readFile(__dirname + path, function(err, data){
        if (err) return send404(res);
        res.writeHead(200, { 'Content-Type' : 'text/css' });
        res.write(data, 'utf8');
        res.end();
      });
      break;
		case '/css/master.css':
			fs.readFile(__dirname + '/css/master.css', function(err, data){
        if (err) return send404(res);
        res.writeHead(200, { 'Content-Type' : 'text/css' });
        res.write(data, 'utf8');
        res.end();
      });
      break;
		case '/':
			fs.readFile(__dirname + '/index.html', function(err, data){
        if (err) return send404(res);
        //res.writeHead(200, {'Content-Type': path == 'json.js' ? 'text/javascript' : 'text/html'})
        res.write(data, 'utf8');
        res.end();
      });
      break;
    default: send404(res);
  }
}),

send404 = function(res){
  res.writeHead(404);
  res.write('404');
  res.end();
};

server.listen(8090);

// socket.io, I choose you
// simplest chat application evar
var io = io.listen(server)
  , buffer = [];

io.set("destroy upgrade",false);
  
io.on('connection', function(client){
	var totalChatGuests = tcpGuests.length;

  client.send({buffer : buffer});
  client.broadcast.send({ announcement: client.sessionId + ' connected' });
  
  chatGuests.push(client);
  client.on('message', function(message){
		totalChatGuests = tcpGuests.length;
    var msg = { message: [client.sessionId, message] };
    buffer.push(msg);
    if (buffer.length > 15) buffer.shift();
    if(totalChatGuests) client.broadcast.send(msg);
    //send msg to tcp connections
    for (g in tcpGuests) {
        tcpGuests[g].write(message);
    }
  });

  client.on('disconnect', function(){
    client.broadcast.send({ announcement: client.sessionId + ' disconnected' });
  });
});

//tcp socket server
var tcpServer = net.createServer(function (socket) {
  console.log('tcp server running on port 1337');
  console.log('web server running on http://localhost:8090');
});

tcpServer.on('connection',function(socket){
    // socket.write('connezcted to the tcp server\r\n');
    console.log('num of connections on port 1337: ' + tcpServer.connections);
    
    tcpGuests.push(socket);
    
    socket.on('data',function(data){
        console.log('received on tcp socket:'+data);
        // socket.write('msg received\r\n');
        
        //send data to guest socket.io chat server
        for (g in io.clients) {
            var client = io.clients[g];
            client.send({message:["arduino",data.toString('ascii',0,data.length)]});
        }
    })
});
tcpServer.listen(1337);