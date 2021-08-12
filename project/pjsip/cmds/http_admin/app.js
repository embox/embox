'use strict';

angular.module("HttpAdmin", ['ngRoute', 'ui.bootstrap'])
.controller("NavBarCtrl", ['$scope', '$location', function($scope, $location) {
    $scope.isActive = function(id) {
        return $location.path().indexOf('/' + id) == 0;
    };
}])
.controller("InterfacesAdminCtrl", ['$scope', '$http', function($scope, $http) {
    $scope.interfaces = [];

    $http.get('cgi-bin/http_admin_iface_list').success(function (data) {
        $scope.interfaces = data;
    });

    $scope.update = function(iface) {
        var post_data = {
            'action' : 'iface_update',
            'data' : iface
        };

        $http.post('cgi-bin/http_admin_iface_list', post_data);
    };

    $scope.flash = function() {
        var post_data = {
            'action' : 'flash_settings',
        };

        $http.post('cgi-bin/http_admin_iface_list', post_data);
    };

}])
.controller("SIPAccountAdminCtrl", ['$scope', '$http', function($scope, $http) {
    $scope.sipAccounts = [];

    $http.get('cgi-bin/http_admin_accounts').success(function (data) {
        $scope.sipAccounts = data;
    });

    $scope.update = function(sip_account) {
        var post_data = {
            'action' : 'accounts_update',
            'data' : sip_account
        };

        $http.post('cgi-bin/http_admin_accounts', post_data);
    };

}])
.controller("LogCtrl", ['$scope', '$http', function($scope, $http) {
    $scope.update = function() {
        $http.get('cgi-bin/cgi_cmd_wrapper?c=loop_file_logger').then(function (r) {
            $scope.logger = r.data;
        });
    };

    $scope.update();
}])
.config(['$routeProvider', function($routeProvider) {
    $routeProvider.
    when('/log', {
        templateUrl: 'partials/log.html',
    }).
    when('/interfaces', {
        templateUrl: 'partials/interfaces.html',
    }).
    when('/accounts', {
        templateUrl: 'partials/accounts.html',
    }).
    otherwise({
        redirectTo: '/interfaces'
    });
}]);

// vim: sw=4 sts=4 expandtab
