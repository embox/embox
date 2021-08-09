
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
      post_data = {
	'action' : 'iface_update',
	'data' : iface
      };

      $http.post('cgi-bin/http_admin_iface_list', post_data).success(function (data) {
	/* saved */
      });
    };

    $scope.flash = function() {
      post_data = {
	'action' : 'flash_settings',
      };

      $http.post('cgi-bin/http_admin_iface_list', post_data).success(function (data) {
	/* saved */
      });
    };

}])
.config(['$routeProvider', function($routeProvider) {
    $routeProvider.
    when('/interfaces', {
        templateUrl: 'partials/interfaces.html',
    }).
    otherwise({
        redirectTo: '/interfaces'
    });
}]);
