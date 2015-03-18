arangosh  \
  --server.endpoint tcp://192.168.173.13:8530  \
  --server.username foo  \
  --server.database test  \
  --server.disable-authentication false

db._create("users");

fred = db.users.save({
    "name":"Fred",
    "gender":"m",
    "age":19,
    "state":"FL"
});

john = db.users.save({
    "name" : "John",
    "gender" : "m",
    "age" : 34
});

carla = db.users.save({
    "name" : "carla",
    "gender" : "f",
    "age": 42,
    "state" : "CA"
});

ana = db.users.save({
    "name" : "ana",
    "gender" : "f",
    "age" : 21,
    "state" : "NJ"
});

stu = db.users.save({
    "name" : "stu",
    "gender" : "m",
    "age" : 45, "likes" : [ "music " ]
});

db._createEdgeCollection("knows");

db.knows.save(john, stu, { "what" : "john knows stu" });
db.knows.save(carla, john, { "what" : "carla knows john" });
db.knows.save(carla, ana, { "what" : "carla knows ana" });
db.knows.save(ana, carla, { "what" : "ana knows carla" });
db.knows.save(carla, fred, { "what" : "carla knows fred" });
db.knows.save(fred, john, { "what" : "fred knows john" });
db.knows.save(fred, stu, { "what" : "fred knows stu" });
db.knows.save(stu, fred, { "what" : "stu knows fred" });

显示所有用户

FOR u IN users RETURN u

查找能认识的所有用户

FOR p IN PATHS(users, knows,'outbound')
   FILTER p.source._id == 'users/6563808665' && LENGTH(p.edges) > 1
   RETURN {"name": p.vertices[*].name, "edges": LENGTH(p.edges)}

FOR p IN PATHS(users, knows,'outbound')
   FILTER p.source._id == 'users/6563808665'
   RETURN {"name": p.vertices[*].name, "edges": LENGTH(p.edges)}