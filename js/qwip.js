$(document).ready(
function(){
  // var myLatlng = new google.maps.LatLng(43.462647, 1.273211);
  // var myOptions = {
  //   zoom: 16,
  //   center: myLatlng,
  //   mapTypeId: google.maps.MapTypeId.SATELLITE
  // };
  // var map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
  // var marker = new google.maps.Marker({
  //   position: myLatlng,
  //       title:"Hello World!"
  //       });
  // marker.setMap(map);

  $.arte({'ajax_url':'/cgi-bin/update_qwip',
	     'ajax_mode':'GET', 
	     //	  'ajax_mode':'POST', 
	  'on_success': on_success,
          'on_data_set': on_data_set}).start();
 $("div.cpu_time").arte("cpu_time");
 $("div.supply").arte("supply");
 $("div.rc_status").arte("radio_control");

 var req = 'fleet';
 $( '[data-role=page]' ).live('pageshow', function(event){ 
     var $et = $( event.target ),
     pageID = 'flight_param';
     //      if( $et.is( pageID )  || $et.children(0).is( pageID ) ){
     req = $et.attr("id");//'flight_param';
	//      }
});

 function on_success(data_xml){
   // look for errors
   var _err = $(data_xml).find('error');
   if (_err.length != 0) {
     jQuery("#footer").html(_err.text());
     return;
   }
   var time_running = $(data_xml).find('time_running').text();
   var my_text = "Connected ".concat(time_running);
   jQuery("#footer").html(my_text);
   
   // print settings
   if (0) {
     var s_txt = "<table>";
     var settings = $(data_xml).find('setting');
     settings.each(function(){
		     var varname = $(this).attr('var');
		     var value = $(this).attr('value');
		     s_txt += "<tr><td>"+varname+"</td><td>"+value+"</td></tr>\n";
		   });
     s_txt += "</table>";
     jQuery("#content").html(s_txt);
   }
   // print fleet
   if (1) {
     jQuery("#header").html("fleet");
     var s_txt = "<table>";
     var aircrafts = $(data_xml).find('aircraft');
     aircrafts.each(function(){
		     var name = $(this).attr('name');
		     s_txt += "<tr><td>"+name+"</td>";
		     s_txt += "<td>"+"<a href=\"#flight_param\" data-role=\"button\">Flight Param</a>"+"</td>\n";
		     s_txt += "<td>"+"<a href=\"#settings\" data-role=\"button\">Settings</a>"+"</td></tr>\n";
		    });
     s_txt += "</table>";
     jQuery("#content").html(s_txt);
   }

   // position on map
   //   var lat = parseFloat($(data_xml).find('latitude').text());    
   //   var lon = parseFloat($(data_xml).find('longitude').text());    
   //   var myLatlng = new google.maps.LatLng(lat, lon);
   //   marker.setPosition(myLatlng);

   // flight param
   //   document.getElementById("cpu_time").text($(data_xml).find('cpu_time').text());

   //   var page_showing = $.mobile.activePage();

 }

 function on_data_set(){
   //var page_showing = $.mobile.activePage();
   //   var ac = "BOOZ2_A7";
   //   var req = "settings";
   //   return "req=" + req + "&ac=" + ac ;
   //   var req = "fleet";
   return "req=" + req;
 }

 $("a[name=center_map]").click(function(e) {
				 map.setCenter(marker.getPosition());
				 e.preventDefault(); 
				 
 });
 
 });


