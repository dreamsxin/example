<?php

$array  = array(
    range(1,6),
    range(7,8),
    'foo' => range(1,3),
    'bar' => range(4,5),
);

// Will output 1 2 3 4 5 6 7 8 1 2 3 4 5
$rait = new RecursiveArrayIterator($array);
$iterator = new RecursiveIteratorIterator(
    $rait,
    RecursiveIteratorIterator::LEAVES_ONLY
);
foreach ($iterator as $val) {
	echo "$val\n";
}

// Will output 6 3 8 5
$rait = new RecursiveArrayIterator($array);
$iterator = new RecursiveIteratorIterator(
    new RecursiveCachingIterator($rait, CachingIterator::CATCH_GET_CHILD),
    RecursiveIteratorIterator::LEAVES_ONLY
);

foreach ($iterator as $val) {
    $lastChild = !$iterator->hasNext();
    if ($lastChild) {
        echo "$val\n";
    }
}

// Will output 5
$rait = new RecursiveArrayIterator($array);
$riit = new RecursiveIteratorIterator(
    $rait,
    RecursiveIteratorIterator::LEAVES_ONLY
);
$iterator = new CachingIterator($riit);
foreach ($iterator as $val) {
    $lastChild = !$iterator->hasNext();
    if ($lastChild) {
        echo "$val is the last child!\n";
    }
}