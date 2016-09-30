'use strict';

angular.module("HttpAdmin", ['ngRoute', 'ui.bootstrap', 'pascalprecht.translate'])
.controller("NavBarCtrl", ['$scope', '$location', '$translate', function($scope, $location, $translate) {
    $scope.isActive = function(id) {
        return $location.path().indexOf('/' + id) == 0;
    };
    $scope.changeLanguage = function(key) {
        $translate.use(key);
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
.controller("LedsCtrl", ['$scope', '$http', '$window', '$modal', '$interval', '$translate',
        function($scope, $http, $window, $modal, $interval, $translate) {

    $scope.leds_state = [];

    function ledStatesFromJson(data) {
        return data.map(function(int_state, index) {
            return {
                index: index,
                state: !!int_state,
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
        var pairs = collectNth(leds_state, 2);
        return pairs.map(function(p, i) {
            return {
                index: i,
                name: "",
                red: p[0],
                blue: p[1]
            }
        });
    }

    $scope.update = function() {
        var leds_state = [];
        $http.get('cgi-bin/cgi_cmd_wrapper?c=led_driver&a1=serialize_states').then(function (r) {
            var data = r.data;
            leds_state = ledStatesArrange(ledStatesFromJson(data));
            return $http.get('cgi-bin/cgi_cmd_wrapper?c=led_names&a1=serialize')
        }).then(function(r) {
            var data = r.data;
            leds_state.forEach(function(p, i) {
                if (i < data.length && data[i]) {
                    p.name = data[i];
                }
            })
            $scope.leds_state = collectNth(leds_state, 2);
        });
    }

    $scope.led_switch = function(led) {
        led.state = !led.state;

        var str_op = led.state ? 'set' : 'clr';
        var uri = 'cgi-bin/cgi_cmd_wrapper?c=led_driver&a1=' + str_op + '&a2=' + led.index;
        $http.get(uri);
    };

    $scope.name_change_request = function(led_pair) {
        var modalInstance = $modal.open({
            templateUrl: 'nameChangeDialog',
            controller: 'NameChangeCtrl',
            resolve: {
                name: function () {
                    return led_pair.name
                }
            }
        });
        modalInstance.result.then(function (new_name) {
            led_pair.name = new_name;
            $http.post('cgi-bin/cgi_cmd_wrapper?c=led_names&a=store&a=' + led_pair.index.toString(), new_name);
        });
    }

    $scope.save = function() {
        $http.get('cgi-bin/cgi_cmd_wrapper?c=flash_settings&a=store&a=led&a=led_names').success(function (data) {
            $translate('CONF_SAVED').then(function (translation) {
                $window.alert(translation);
            });
        });
    };

    $scope.update_errors = function() {
        $http.get('cgi-bin/cgi_cmd_wrapper?c=led_driver&a1=serialize_errors').success(function(data) {
            $scope.leds_state.forEach(function(p) {
                p.forEach(function(p2) {
                    p2.error = data[p2.red.index] || data [p2.blue.index];
                })
            })
        });
    };

    $scope.update();

    var error_update = $interval(function() { $scope.update_errors() }, 2000);
    $scope.$on('$destroy', function () {
        $interval.cancel(error_update);
    });
}])
.controller("NameChangeCtrl", [ '$scope', '$modalInstance', 'name', function ($scope, $modalInstance, name) {

    $scope.name = name;

    $scope.ok = function () {
        $modalInstance.close($scope.name);
    };

    $scope.cancel = function () {
        $modalInstance.dismiss('cancel');
    };
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
.config(['$translateProvider', function($translateProvider) {
    $translateProvider.translations('en', {
        'PANEL_TITLE' : 'Embox Panel',
        'NETWORK' : 'Network',
        'LEDS' : 'Leds',
        'LANG' : 'Lang',
        'RED' : 'Red',
        'BLUE' : 'Blue',
        'REFRESH' : 'Refresh',
        'SAVE' : 'Save',
        'CONF_SAVED' : 'Led configuration saved!',
        'IP' : 'IP address',
        'NETMASK' : 'Netmask',
        'MAC' : 'MAC address',
        'APPLY_SETTINGS' : 'Apply settings',
        'CHANGE_TITLE' : 'Change block name',
        'NAME' : 'Name',
        'REMAIN' : 'remain',
        'OK' : 'OK',
        'CANCEL' : 'Cancel'
    });
    $translateProvider.translations('ru', {
        'PANEL_TITLE' : 'Панель Embox',
        'NETWORK' : 'Сеть',
        'LEDS' : 'Управление',
        'LANG' : 'Язык',
        'RED' : 'Красный',
        'BLUE' : 'Синий',
        'REFRESH' : 'Обновить',
        'SAVE' : 'Сохранить',
        'CONF_SAVED' : 'Кофигурация сохранена!',
        'IP' : 'IP адрес',
        'NETMASK' : 'Маска подсети',
        'MAC' : 'MAC адрес',
        'APPLY_SETTINGS' : 'Применить',
        'CHANGE_TITLE' : 'Изменение имени блока',
        'NAME' : 'Имя',
        'REMAIN' : 'осталось',
        'OK' : 'OK',
        'CANCEL' : 'Отменить'
    });
    $translateProvider.preferredLanguage('en');
}])
.directive('ledTemplate', function() {
    return {
        restrict: 'E',
        scope: {
            led: '=led',
            led_on: '@ledOn',
            led_switch: '&ledSwitch',
            led_name: '@ledName'
        },
        templateUrl:"LedTemplate"
    };
});

// vim: sw=4 sts=4 expandtab
