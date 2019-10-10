'use strict';

angular.module("HttpAdmin", ['ngRoute', 'ui.bootstrap'])
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
    $scope.leds = [];

    function ledStatesFromJson(data) {
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
        var uri = 'cgi-bin/cgi_cmd_wrapper?c=led_driver&a1=' + str_op + '&a2=' + led.index;
        $http.get(uri);
    };

    $scope.save = function() {
        $http.get('cgi-bin/cgi_cmd_wrapper?c=flash_settings&a=store&a=led').success(function (data) {
            alert('Led configuration saved!');
        });
    };

    $scope.update = function() {
        $http.get('cgi-bin/cgi_cmd_wrapper?c=led_driver&a1=serialize_states').then(function (r) {
            var data = r.data;
            $scope.leds = ledStatesFromJson(data);
        });
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
}]);

// vim: sw=4 sts=4 expandtab
