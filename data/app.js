var app = angular.module('config_app', []);

app.controller('WiFiSettingCtr', function ($scope, $http) {
    var wifiSetting = this;
    wifiSetting.selected_ssid = "";
    wifiSetting.ssid_password = "";
    wifiSetting.availableNetworks = [];

    wifiSetting.getAvailableNetwork = function () {
        $http({
            method: "GET",
            url: "/api/getssid"
        }).then(function success(response) {
            wifiSetting.availableNetworks = response.data.ssids;
        }, function error(response) {
            //$scope.myWelcome = response.statusText;
        });
    };
    
    wifiSetting.saveNetworkSetting = function () {
        $http({
            method: "POST",
            url: "/api/save/setting/nw",
            data: { selected_ssid: wifiSetting.selected_ssid , ssid_password : wifiSetting.ssid_password}
        }).then(function success(response) {
            console.log(response);
        }, function error(response) {
            //$scope.myWelcome = response.statusText;
        });
    };
    
    wifiSetting.finishWizard = function () {
        $http({
            method: "GET",
            url: "/api/finish",
        }).then(function success(response) {
            console.log(response);
        }, function error(response) {
            //$scope.myWelcome = response.statusText;
        });
    };
    
    wifiSetting.getAvailableNetwork();

});

app.controller('RebootCtr', function ($scope, $http) {
    var rebootCtr = this;
    rebootCtr.finishWizard = function () {
        $http({
            method: "GET",
            url: "/api/finish",
        }).then(function success(response) {
            console.log(response);
        }, function error(response) {
            //$scope.myWelcome = response.statusText;
        });
    };
});