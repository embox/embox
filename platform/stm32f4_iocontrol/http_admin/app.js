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
        post_data = {
            'action' : 'iface_update',
            'data' : iface
        };

        $http.post('cgi-bin/http_admin_backend', post_data).success(function (data) {
            /* saved */
        });
    };

    $scope.flash = function() {
        post_data = {
            'action' : 'flash_settings',
        };

        $http.post('cgi-bin/http_admin_backend', post_data).success(function (data) {
            /* saved */
        });
    };

}])
.controller("LedsCtrl", ['$scope', '$http', function($scope, $http) {
    $scope.leds_state = [];

    $scope.update = function() {
        $http.get('cgi-bin/cgi_cmd_wrapper?c=led_driver&a1=serialize_states').success(function (data) {
            $scope.leds_state = data;
        });
    }

    $scope.led_switch = function(led_n) {
        $scope.leds_state[led_n] = !$scope.leds_state[led_n];

        var str_op = $scope.leds_state[led_n] ? 'set' : 'clr';
        $http.get('cgi-bin/cgi_cmd_wrapper?c=led_driver&a1=' + str_op + '&a2=' + led_n).success(function (data) {

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
