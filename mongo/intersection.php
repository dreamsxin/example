<?php

$m = new MongoClient();
$db = $m->selectDB ("test");

$c = $db->selectCollection ("test");

$data = array(
     'name'  =>  'name1' ,
     'phone'  =>  '13611111111' ,
     'friends'  => array('13611111112',  '13611111113',  '13611111114'),
);

$c->insert($data , array("w"  => 1));

$data = array(
     'name'  =>  'name2' ,
     'phone'  =>  '13611111112' ,
     'friends'  => array('13611111113',  '13611111114',  '13611111115'),
);

$c->insert($data , array("w"  => 1));

$data = array(
     'name'  =>  'name3' ,
     'phone'  =>  '13611111113' ,
     'friends'  => array('13611111114',  '13611111115',  '13611111116'),
);

$c->insert($data , array("w"  => 1));

$data = array(
     'name'  =>  'name4' ,
     'phone'  =>  '13611111114' ,
     'friends'  => array('13611111115',  '13611111116',  '13611111117'),
);

$c->insert($data , array("w"  => 1));
$friends = array( 'friends' => array('13611111115', '13611111117'));

// 交集
$js = new MongoCode("function() {
	var ret = false
	this.friends.forEach(function(friend) {
		if (Array.contains(friends, friend)) {
			ret = true;
		}
	});
	return ret;
}", $friends);

$cursor = $c->find(array( '$where'  =>  $js ));

// 交集个数
$map = new  MongoCode ("function() {
	var key = {_id:this._id, name:this.name, phone:this.phone};
	this.friends.forEach(function(friend) { 
		emit(key, friend);
	});
}");

$reduce =  new  MongoCode ("function(phone, vals) {
	var sum = 0;
	for (var i in vals) {
		if (Array.contains(friends, vals[i])) {
			sum += 1;
		}
	}
	return sum;
}", $friends);

$query = array( '$where'  => new MongoCode("function() {
	var ret = false
	for (var i in this.friends) {
		if (Array.contains(friends, this.friends[i])) {
			ret = true;
			break;
		}
	}
	return ret;
}", $friends)); 

// Map Reduce
$ret = $db ->command (array(
     "mapreduce"  =>  "images" , 
     "map"  =>  $map ,
     "reduce"  =>  $reduce,
	 "query"  => $query,
	 "out" => 'temp',
));

$users = $db->selectCollection($ret['result'])->find();


// 聚合 aggregate
$di = new Phalcon\DI\FactoryDefault;

$di->set('collectionManager', function() {
    $modelsManager = new Phalcon\Mvc\Collection\Manager();
    return $modelsManager;
}, true);

$di->set('mongo', function(){
	$m = new MongoClient();
	return $m->selectDB('test');
}, true);

class Contacts extends \Phalcon\Mvc\Collection {

	public $user_id;
	public $contact_user_id;
	public $group_id;
}

$contact = new Contacts;
$contact->user_id = 1;
$contact->contact_user_id = 2;
$contact->save();

$contact = new Contacts;
$contact->user_id = 1;
$contact->contact_user_id = 3;
$contact->save();

$contact = new Contacts;
$contact->user_id = 4;
$contact->contact_user_id = 2;
$contact->save();

$contact = new Contacts;
$contact->user_id = 4;
$contact->contact_user_id = 3;
$contact->save();

$ops = array(
	array(
		'$match'  => array(
			"contact_user_id"  => array( '$in'  =>  array(2, 3) ),
			"user_id"  => array( '$ne'  =>  1 ),
		),
	),
	array(
		'$group'  => array(
			"_id"  => array( "user_id"  =>  '$user_id' ),
			"friends"  => array( '$sum'  =>  1 ),
		),
	),
);

$results = $contact->aggregate($ops);