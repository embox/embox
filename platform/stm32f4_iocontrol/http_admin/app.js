'use strict';

angular.module("HttpAdmin", ['ngRoute'])
.controller("NavBarCtrl", ['$scope', '$location', function($scope, $location) {
    $scope.isActive = function(id) {
        return $location.path().indexOf('/' + id) == 0;
    };
}])
.controller("InterfacesAdminCtrl", ['$scope', '$http', function($scope, $http) {
    $scope.interfaces = [];

    $http.get('cgi-bin/http_admin_backend').success(function (data) {
        $scope.interfaces = data;
    });

    $scope.update = function(iface) {
        var post_data = {
            'action' : 'iface_update',
            'data' : iface
        };

        $http.post('cgi-bin/http_admin_backend', post_data);
    };

    $scope.flash = function() {
        var post_data = {
            'action' : 'flash_settings',
        };

        $http.post('cgi-bin/http_admin_backend', post_data);
    };

}])
.controller("LedsCtrl", ['$scope', '$http', function($scope, $http) {
    function ledStatesFromJson(data) {
        return data.map(function(int_state, index) {
            return {
                index: index,
                state: !!int_state
            };
        });
    };

    function collectNth(a, n) {
        return a.reduce(function(base, x, index) {
            if (index % n == 0) {
                base.push([x]);
            } else {
                base[base.length - 1].push(x);
            }
            return base;
        }, []);
    };

    function ledStatesArrange(leds_state) {
        return collectNth(collectNth(leds_state, 2), 2);
    }

    $scope.leds_state = [];

    $scope.update = function() {
        $http.get('cgi-bin/cgi_cmd_wrapper?c=led_driver&a1=serialize_states').success(function (data) {
            $scope.leds_state = ledStatesArrange(ledStatesFromJson(data));
        });
    }

    $scope.led_switch = function(led) {
        led.state = !led.state;

        var str_op = led.state ? 'set' : 'clr';
        var uri = 'cgi-bin/cgi_cmd_wrapper?c=led_driver&a1=' + str_op + '&a2=' + led.index;
        $http.get(uri);
    };

    $scope.save = function() {
        $http.get('cgi-bin/cgi_cmd_wrapper?c=flash_settings&a1=store&a2=led');
    };

    $scope.update();
}])
.config(['$routeProvider', function($routeProvider) {
    $routeProvider.
    when('/interfaces', {
        templateUrl: 'partials/interfaces.html',
    }).
    when('/leds', {
        templateUrl: 'partials/leds.html',
    }).
    otherwise({
        redirectTo: '/leds'
    });
}])
.directive('blockTemplate', function() {
    return {
        restrict: 'E',
        scope: {
            led_pair: '=ledPair',
            led_switch: '&ledSwitch'
        },
        templateUrl:"BlockTemplate"
    };
});

// vim: sw=4 sts=4 expandtab
