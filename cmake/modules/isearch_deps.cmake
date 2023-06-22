SET(github_base_uri https://github.com/sebbekarlsson)

function (isearch_init_deps TARGET_NAME)

  include(FetchContent)

  if (NOT TARGET mif_static)

    FetchContent_Declare(
      mif_static
      GIT_REPOSITORY ${github_base_uri}/mif.git 
    )

    FetchContent_MakeAvailable(mif_static)
  endif()

  if (NOT TARGET spath_static)

    FetchContent_Declare(
      spath_static
      GIT_REPOSITORY ${github_base_uri}/spath.git 
    )

    FetchContent_MakeAvailable(spath_static)
  endif()

  if (NOT TARGET hashy_static)

    FetchContent_Declare(
      hashy_static
      GIT_REPOSITORY ${github_base_uri}/hashy.git 
    )

    FetchContent_MakeAvailable(hashy_static)
  endif()

  if (NOT TARGET bytepager_static)

    FetchContent_Declare(
      bytepager_static
      GIT_REPOSITORY ${github_base_uri}/bytepager.git 
    )

    FetchContent_MakeAvailable(bytepager_static)
  endif()

  if (NOT TARGET arena_static)

    FetchContent_Declare(
      arena_static
      GIT_REPOSITORY ${github_base_uri}/arena.git 
    )

    FetchContent_MakeAvailable(arena_static)
  endif()


  set(isearch_deps
    m
    bytepager_static
    hashy_static
    arena_static
    mif_static
    spath_static
  )

  target_compile_options(bytepager_static PUBLIC -fPIC)
  target_link_libraries(${TARGET_NAME} PUBLIC ${isearch_deps})
endfunction()
