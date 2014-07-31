
angular.module("HttpAdmin", [])
  .controller("InterfacesAdminCtrl", ['$scope', '$http', function($scope, $http) {
    $scope.interfaces = [];

    $http.get('cgi-bin/http_admin_backend').success(function (data) {
      $scope.interfaces = data;
    });

    $scope.update = function(iface) {
      $http.post('cgi-bin/http_admin_backend', iface).success(function (data) {
	/* saved */
      });
    };

  }]);
