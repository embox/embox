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
}])
.controller("SystemCtrl", ['$scope', '$http', function($scope, $http) {
    $scope.version = null;
    $scope.date = null;
    $scope.threads = null;

    $scope.update = function() {
        $http.get('cgi-bin/cgi_cmd_wrapper?c=date').then(function (r) {
            $scope.date = r.data;
        });
        $http.get('cgi-bin/cgi_cmd_wrapper?c=version').then(function (r) {
            $scope.version = r.data;
        });
        $http.get('cgi-bin/cgi_cmd_wrapper?c=thread&a=-s').then(function (r) {
            $scope.threads = r.data;
        });
    };

    $scope.update();
}])
.config(['$routeProvider', function($routeProvider) {
    $routeProvider.
    when('/interfaces', {
        templateUrl: 'partials/interfaces.html',
    }).
    when('/system', {
        templateUrl: 'partials/system.html',
    }).
    otherwise({
        redirectTo: '/interfaces'
    });
}]);

// vim: sw=4 sts=4 expandtab
