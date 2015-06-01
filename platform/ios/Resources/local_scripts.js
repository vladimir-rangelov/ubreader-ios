var g_cover_images_url = new Array();
var g_cover_images_parent_ids = new Array();
var g_current_loaded_img_index = 0;

function on_windows_resize(){
    if(!g_shelf_fiew) return;
    
    var s_width =  window.innerWidth;
    var s_height = window.innerHeight;
    var book_number =Math.floor(s_width / g_cover_width) ;
    s_width -= g_parasite_padding ;
    s_width /= book_number;
    $(".book-element-shelf").css('width', s_width+'px');
}

$(document).ready(function () {
                  
                  $("img").show().lazyload();
                  $("img").lazyload({
                                    threshold : 400,
                                    skip_invisible : false
                                    });
                  
                  on_windows_resize();
                  $(window).resize(on_windows_resize);
                  
                });


function move_books_to_shelf_view(){
    $("#books")
    .appendTo("#shelf_container");
    $("#books").css('background',"url(" + shelf_background + ")");
    $( "#list_container" ).hide();
    $( "#shelf_top" ).show();
    $( "#shelf_table" ).show();
    $(".book-element-list").attr('class', 'book-element-shelf');
    $(".book-cover-list").attr('class', 'book-cover-shelf');
    $(".book-info-list").attr('class', 'book-info-shelf');
    on_windows_resize();
}

function move_books_to_list_view() {
    $("#books")
    .appendTo("#list_container");
    $("#books").css('background',"transparent");
    $("#list_container" ).show();
    $("#shelf_top" ).hide();
    $("#shelf_table" ).hide();
    $(".book-element-shelf").attr('class', 'book-element-list');
    $(".book-cover-shelf").attr('class', 'book-cover-list');
    $(".book-info-shelf").attr('class', 'book-info-list');
    $(".book-element-list").css('width',"100%");
    $("tr:odd").css("background-color", "LightGray");
}

var g_shelf_fiew= true;
function switch_view_type(){
    g_shelf_fiew =! g_shelf_fiew;
    
    if(g_shelf_fiew){
        move_books_to_shelf_view();
    }else {
        move_books_to_list_view();
    }
}


function load_image(index) {
    
    /*$.ajax({
           url : _url,
           processData : false
           }).always(function(b64data){
                     var img = $("<img />");
                     img.attr('class', "book-cover-shelf");
                     img.attr("src", b64data);
                     $(parent_element_id).append(img);
                     });*/
    
    /*$.ajax({
           type: "GET",
           url: _url,
           dataType: "image/png",
           success: function(img) {
           i = new Image();
           i.src = img;
           alert(i);
           $(parent_element_id).append(i);
           },
           error: function(error, txtStatus) {
           alert(txtStatus);
           console.log(txtStatus);
           console.log('error');
           }
           });*/
    
    
    var img = $("<img />");
    img.attr('src', g_cover_images_url[index]);
    img.attr('class', "book-cover-shelf");
    
    var parent_id = g_cover_images_parent_ids[index];
    img.load(function() {
        if (!this.complete || typeof this.naturalWidth == "undefined" || this.naturalWidth == 0) {
             alert('broken image!');
        } else {
             $(parent_id).append(img);
             if (index < g_cover_images_url.length - 1)
                load_image(++index);
                //alert(index);
        }
    });
}

function cache_image(url, parent_element_id) {
    
    g_cover_images_url[g_current_loaded_img_index] = url;
    g_cover_images_parent_ids[g_current_loaded_img_index] = parent_element_id;
    g_current_loaded_img_index++;
    
}
























