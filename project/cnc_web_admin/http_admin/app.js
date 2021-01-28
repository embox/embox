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
.controller("CncCtrl", ['$scope', '$http', function($scope, $http) {
    $scope.live = {};

    // handles the callback from the received event
    var eventCallback = function (msg) {
        $scope.$apply(function () {
            $scope.live = JSON.parse(msg.data)
        });
    }

    var source = new EventSource('/cgi-bin/live_status');
    source.addEventListener('message', eventCallback);
    $scope.$on('$destroy', function () {
        source.close();
    });

    $scope.run = function() {
        $http.get('cgi-bin/cnc_manager?run');
    };
}])
.config(['$routeProvider', function($routeProvider) {
    $routeProvider.
    when('/interfaces', {
        templateUrl: 'partials/interfaces.html',
    }).
    when('/cnc', {
        templateUrl: 'partials/cnc.html',
    }).
    otherwise({
        redirectTo: '/cnc'
    });
}]);

// vim: sw=4 sts=4 expandtab
