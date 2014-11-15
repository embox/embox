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

        $http.post('cgi-bin/http_admin_backend', post_data).success(function (data) {
            /* saved */
        });
    };

    $scope.flash = function() {
        var post_data = {
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
            var led_n = data.length;
            var new_leds_state = [];
            for (var i = 0; i < led_n; i += 2) {
                var state = { red : !!data[i], blue : !!data[i + 1] };
                new_leds_state.push(state);
            }
            $scope.leds_state = new_leds_state;
        });
    }

    $scope.led_switch = function(state, pair_n, offset) {
        var led_n = pair_n * 2 + offset;
        var new_state;

        if (offset == 0) {
            state.red = !state.red;
            new_state = state.red;
        } else {
            state.blue = !state.blue;
            new_state = state.blue;
        }

        var str_op = new_state ? 'set' : 'clr';
        $http.get('cgi-bin/cgi_cmd_wrapper?c=led_driver&a1=' + str_op + '&a2=' + led_n).success(function (data) {

        });
    };

    $scope.save = function() {
        $http.get('cgi-bin/cgi_cmd_wrapper?c=flash_settings&a1=store&a2=led').success(function (data) {

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
