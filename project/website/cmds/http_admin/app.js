'use strict';

var app = angular.module("HttpAdmin", ['ngRoute', 'ui.bootstrap'])

app.run();

app.controller("NavBarCtrl", ['$scope', '$location', function($scope, $location) {
}]);

app.controller('DemoCtrl', ['$scope', '$http', function($scope, $http) {
    $scope.leds = [];
    $scope.light = null;
    $scope.rate = null;

    var eventCallback = function (msg) {
        $scope.$apply(function () {
            $scope.rate = JSON.parse(msg.data).rate
        });
    }

    var source = new EventSource('/cgi-bin/get_angular_rate');
    source.addEventListener('message', eventCallback);
    $scope.$on('$destroy', function () {
        source.close();
    });
  
    function ledStatesFromJson(data) {
        console.log('input data: ' + data);

        return data.map(function(int_state, index) {
            return {
                index: index,
                active: !!int_state,
            };
        });
    };
  
    $scope.led_toggle = function(led) {
        led.active = !led.active;
    
        var str_op = led.active ? 'set' : 'clr';
        var uri = 'cgi-bin/led_driver?cmd=' + str_op + '&led=' + led.index;
        $http.get(uri);
    };

    $scope.update = function() {
        $http.get('cgi-bin/led_driver?cmd=serialize_states').then(function (r) {
            var data = r.data;
            $scope.leds = ledStatesFromJson(data);
        });
    };

    $scope.update();
}]);

app.controller("SystemCtrl", ['$scope', '$http', function($scope, $http) {
    $scope.version = null;
    $scope.time = null;
    $scope.tasks = null;

    var eventCallbackTime = function (msg) {
        $scope.$apply(function () {
            $scope.time = JSON.parse(msg.data).time
        });
    }

    var eventCallbackTasks = function (msg) {
        $scope.$apply(function () {
            $scope.tasks = msg.data
        });
    }

    var source_time = new EventSource('/cgi-bin/gettime');
    source_time.addEventListener('message', eventCallbackTime);

    var source_tasks = new EventSource('/cgi-bin/gettasks');
    source_tasks.addEventListener('message', eventCallbackTasks);

    $scope.$on('$destroy', function () {
        source_time.close();
        source_tasks.close();
    });

    $scope.update = function() {
        $scope.version = "Version 0.1";
    };

    $scope.update();
}]);

app.controller("InfoCtrl", ['$scope', '$http', function($scope, $http) {
    $scope.update = function() {
    };

    $scope.update();
}]);

app.config(function($routeProvider, $locationProvider) {
    $routeProvider.
    when('/demo', {
        templateUrl: 'partials/demo.html',
    }).
    when('/system', {
        templateUrl: 'partials/system.html',
    }).
    when('/info', {
        templateUrl: 'partials/info.html',
    }).
    otherwise({
        redirectTo: '/info'
    });
});
